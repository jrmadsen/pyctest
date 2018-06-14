#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import argparse
import traceback

import tomopy
import dxchange
import tornado
import matplotlib
import timemory
import timemory.options as options
import signal
import numpy as np
import time as t
import pylab


#------------------------------------------------------------------------------#
algorithms = [ 'gridrec', 'art', 'fbp', 'bart', 'mlem', 'osem', 'sirt',
               'ospml_hybrid', 'ospml_quad', 'pml_hybrid', 'pml_quad' ]


#------------------------------------------------------------------------------#
def convert_image(fname, current_format, new_format):

    _fext = new_format
    _success = True

    try:

        from PIL import Image
        _cur_img = "{}.{}".format(fname, current_format)
        img = Image.open(_cur_img)
        out = img.convert("RGB")
        out.save(fname, "jpeg", quality=95)
        print("  --> Converted '{}' to {} format...".format(fname, new_format.upper()))

    except Exception as e:

        print("  --> ##### {}...".format(e))
        print("  --> ##### Exception occurred converting '{}' to {} format...".format(
            fname, new_format.upper()))

        _fext = current_format
        _success = False

    _fname = "{}.{}".format(fname, _fext)
    return [ _fname, _success, _fext ]


#------------------------------------------------------------------------------#
def exit_action(errcode):
    man = timemory.manager()
    timemory.report(ign_cutoff=True)
    fname = 'signal_error_{}.out'.format(errcode)
    f = open(fname, 'w')
    f.write('{}\n'.format(man))
    f.close()


#------------------------------------------------------------------------------#
def output_image(image, fname):

    img = pylab.imsave(fname, image, cmap='gray')

    if os.path.exists(fname):
        print("  --> Image file found @ '{}'...".format(fname))
    else:
        print("  ##################### WARNING #####################")
        print("  --> No image file at @ '{}' (expected) ...".format(fname))


#------------------------------------------------------------------------------#
def rescale_image(rec, nimages, scale, transform=True):

    rec_n = rec.copy()
    try:
        _min = np.amin(rec_n)
        rec_n -= _min
        _max = np.amax(rec_n)
        if _max > 0.0:
            rec_n /= 0.5 * _max
        rec_n -= 1
        import skimage.transform
        if transform is True:
            _nrows = rec[0].shape[0] * scale
            _ncols = rec[0].shape[1] * scale
            rec_tmp = np.ndarray([nimages, _nrows, _ncols])
            for i in range(nimages):
                rec_tmp[i] = skimage.transform.resize(rec_n[i],
                    (rec_n[i].shape[0] * scale, rec_n[i].shape[1] * scale))
            rec_n = rec_tmp

    except Exception as e:
        print("  --> ##### {}...".format(e))
        rec_n = rec.copy()

    return rec_n


#------------------------------------------------------------------------------#
@timemory.util.auto_timer()
def output_images(rec, fpath, format="jpeg", scale=1, ncol=1):

    imgs = []
    nitr = 0
    nimages = rec.shape[0]
    rec_i = None
    fname = "{}".format(fpath)

    if nimages < ncol:
        ncol = nimages

    rec_n = rec.copy()
    if scale > 1:
        rescale_image(rec, nimages, scale)

    print("Image size: {} x {} x {}".format(
        rec[0].shape[0],
        rec[0].shape[1],
        rec.shape[0]))

    print("Scaled Image size: {} x {} x {}".format(
        rec_n[0].shape[0],
        rec_n[0].shape[1],
        rec_n.shape[0]))

    for i in range(nimages):
        nitr += 1;

        _f = "{}{}".format(fpath, i)
        _fimg = "{}.{}".format(_f, format)

        if rec_i is None:
            rec_i = rec_n[i]
        else:
            rec_i = np.concatenate((rec_i, rec_n[i]), axis=1)

        if nitr % ncol == 0 or i+1 == nimages:
            fname = "{}{}.{}".format(fname, i, format)
            output_image(rec_i, fname)
            imgs.append(fname)
            rec_i = None
            fname = "{}".format(fpath)
        else:
            fname = "{}{}_".format(fname, i)

    return imgs


#------------------------------------------------------------------------------#
@timemory.util.auto_timer()
def generate(phantom = "shepp3d", nsize = 512, nangles = 360):

    with timemory.util.auto_timer("[tomopy.misc.phantom.{}]".format(phantom)):
        obj = getattr(tomopy.misc.phantom, phantom)(size=nsize)
    with timemory.util.auto_timer("[tomopy.angles]"):
        ang = tomopy.angles(nangles)
    with timemory.util.auto_timer("[tomopy.project]"):
        prj = tomopy.project(obj, ang)

    #np.save('projection.npy', prj)
    #np.save('angles.npy', ang)

    return [ prj, ang, obj ]


#------------------------------------------------------------------------------#
@timemory.util.auto_timer()
def run(phantom, algorithm, size, nangles, ncores, format, scale, ncol, get_recon = False):

    nitr = size
    ndigits = 6
    imgs = []
    fname = os.path.join(os.getcwd(), phantom)
    fname = os.path.join(fname, algorithm)
    fname = os.path.join(fname, "stack_{}_".format(algorithm))

    prj, ang, obj = generate(phantom, size, nangles)

    if nitr != prj.shape[0]:
        nitr = prj.shape[0]

    with timemory.util.auto_timer("[tomopy.recon(algorithm='{}')]".format(algorithm)):
        rec = tomopy.recon(prj, ang, algorithm=algorithm, ncore=ncores)

    if get_recon is True:
        return rec

    imgs.extend(output_images(rec, fname, format, scale, ncol))

    return imgs


#------------------------------------------------------------------------------#
def main(args):

    manager = timemory.manager()

    algorithm = args.algorithm
    if len(args.compare) > 0:
        algorithm = "comparison"

    print('\nArguments:\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n'.format(
        "\tPhantom", args.phantom,
        "\tAlgorithm", algorithm,
        "\tSize", args.size,
        "\tAngles", args.angles,
        "\t# of cores", args.ncores,
        "\tFormat", args.format,
        "\tScale", args.scale,
        "\t# of columns", args.ncol,
        "\tcomparison", args.compare))

    if len(args.compare) > 0:
        args.ncol = 1
        args.scale = 1
        arr = None
        _nrows = None
        _ncols = None
        _nitr = 0
        seq = None
        for alg in args.compare:
            print("Reconstructing with {}...".format(alg))
            if seq is None:
                seq = "{}".format(alg)
            else:
                seq = "{}-{}".format(seq, alg)
            tmp = run(args.phantom, alg, args.size, args.angles,
                      args.ncores, args.format, args.scale, args.ncol,
                      get_recon=True)
            tmp = rescale_image(tmp, args.size, args.scale, transform=False)
            _nrow = tmp[0].shape[0]
            _ncol = tmp[0].shape[1]
            if arr is None:
                _nrows = _nrow
                _ncols = _ncol * len(args.compare)
                arr = np.ndarray([tmp.shape[0], _nrows, _ncols], dtype=float)
            _b = (_ncol*_nitr)
            _nitr += 1
            _e = (_ncol*_nitr)
            arr[:, :, _b:_e] = tmp[:,:,:]
        #
        print("Total array size: {} x {} x {}".format(
            arr[0].shape[0],
            arr[0].shape[1],
            arr.shape[0]))

        fname = os.path.join(os.getcwd(), args.phantom)
        fname = os.path.join(fname, algorithm)
        fname = os.path.join(fname, "stack_{}_".format(seq))
        imgs = output_images(arr, fname,
                             args.format, args.scale, args.ncol)
    else:
        print("Reconstructing with {}...".format(args.algorithm))
        imgs = run(args.phantom, args.algorithm, args.size, args.angles,
                   args.ncores, args.format, args.scale, args.ncol)


    # timing report to stdout
    print('{}'.format(manager))

    timemory.options.output_dir = "./{}".format(os.path.join(args.phantom, algorithm))
    timemory.options.set_report("run_tomopy.out")
    timemory.options.set_serial("run_tomopy.json")
    manager.report()

    #------------------------------------------------------------------#
    # provide timing plots
    try:
        timemory.plotting.plot(files=[timemory.options.serial_filename], echo_dart=True,
                               output_dir=timemory.options.output_dir)
    except Exception as e:
        print("Exception - {}".format(e))

    #------------------------------------------------------------------#
    # provide results to dashboard
    try:
        for i in range(0, len(imgs)):
            img_base = "{}_{}_stack_".format(args.phantom, algorithm, i)
            img_name = os.path.basename(imgs[i]).replace(
                ".{}".format(args.format), "").replace(
                "stack_{}_".format(algorithm), img_base)
            img_type = args.format
            img_path = imgs[i]
            timemory.plotting.echo_dart_tag(img_name, img_path, img_type)
    except Exception as e:
        print("Exception - {}".format(e))

    #------------------------------------------------------------------#
    # provide ASCII results
    try:
        notes = manager.write_ctest_notes(directory="{}/{}".format(args.phantom, algorithm))
        print('"{}" wrote CTest notes file : {}'.format(__file__, notes))
    except Exception as e:
        print("Exception - {}".format(e))


#------------------------------------------------------------------------------#
if __name__ == "__main__":

    parser = argparse.ArgumentParser()

    import multiprocessing as mp
    ncores = mp.cpu_count()

    parser.add_argument("-p", "--phantom", help="Phantom to use",
                        default="shepp3d", type=str)
    parser.add_argument("-a", "--algorithm", help="Select the algorithm",
                        default="gridrec", choices=algorithms, type=str)
    parser.add_argument("-A", "--angles", help="number of angles",
                        default=360, type=int)
    parser.add_argument("-s", "--size", help="size of image",
                        default=512, type=int)
    parser.add_argument("-n", "--ncores", help="number of cores",
                        default=ncores, type=int)
    parser.add_argument("-f", "--format", help="output image format",
                        default="jpeg", type=str)
    parser.add_argument("-S", "--scale", help="scale image by a positive factor",
                        default=1, type=int)
    parser.add_argument("-c", "--ncol", help="Number of images per row",
                        default=1, type=int)
    parser.add_argument("--compare", help="Generate comparison",
                        nargs='*', default=["none"], type=str)

    args = timemory.options.add_args_and_parse_known(parser)

    if len(args.compare) == 1 and args.compare[0].lower() == "all":
        args.compare = list(algorithms)
    elif len(args.compare) == 1:
        args.compare = []

    pdir = os.path.join(os.getcwd(), args.phantom)
    if not os.path.exists(pdir):
        os.makedirs(pdir)

    alg = args.algorithm
    if len(args.compare) > 0:
        alg = "comparison"

    adir = os.path.join(pdir, alg)
    if not os.path.exists(adir):
        os.makedirs(adir)

    if len(args.compare) == 0:
        try:
            import shutil
            if os.path.exists(adir):
                shutil.rmtree(adir)
                os.makedirs(adir)
        except:
            pass
    else:
        try:
            import shutil
            if os.path.exists(adir):
                shutil.rmtree(adir)
                os.makedirs(adir)
        except:
            pass


    ret = 0
    try:

        with timemory.util.timer('\nTotal time for "{}"'.format(__file__)):
            main(args)

    except Exception as e:
        exc_type, exc_value, exc_traceback = sys.exc_info()
        traceback.print_exception(exc_type, exc_value, exc_traceback, limit=5)
        print ('Exception - {}'.format(e))
        ret = 2
        os.kill(os.getpid(), signal.SIGHUP)

    sys.exit(ret)
