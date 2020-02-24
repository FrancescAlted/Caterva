/*
 * Copyright (C) 2018 Francesc Alted, Aleix Alcacer.
 * Copyright (C) 2019-present Blosc Development team <blosc@blosc.org>
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */

#include <caterva.h>

int main(){
    // Create a context
    blosc2_cparams cparams = BLOSC2_CPARAMS_DEFAULTS;
    cparams.compcode = BLOSC_BLOSCLZ;
    caterva_ctx_t *ctx = caterva_new_ctx(NULL, NULL, cparams, BLOSC2_DPARAMS_DEFAULTS);
    ctx->cparams.typesize = sizeof(double);

    // Define the partition shape for the first array
    const int8_t ndim = 3;
    const int64_t dimx = 500, dimy = 500, dimz = 500;
    int64_t shape_[] = {dimx, dimy, dimz};
    int64_t pshape_[] = {64, 64, 64};
    int64_t spshape_[] = {16, 16, 16};
    // int64_t start_[] = {0, 0, 0};  // 3-d slice
    // int64_t start_[] = {dimx - 1, 0, 0};  // 2-d slice
    int64_t start_[] = {0, dimy - 1, 0};  // 2-d slice
    // int64_t start_[] = {dimx - 1, dimy - 1, 0};  // 1-d slice
    int64_t stop_[] = {dimx, dimy, dimz};

    blosc_timestamp_t last, current;
    caterva_dims_t shape = caterva_new_dims(shape_, ndim);
    caterva_dims_t pshape = caterva_new_dims(pshape_, ndim);
    caterva_dims_t spshape = caterva_new_dims(spshape_, ndim);

    int64_t buf_size = 1;
    for (int i = 0; i < ndim; ++i) {
        buf_size *= (shape_[i]);
    }
    double *buf_src = (double *) malloc((size_t)buf_size * ctx->cparams.typesize);
    for (int64_t i = 0; i < buf_size; ++i) {
        buf_src[i] = (double) i;
    }

    caterva_array_t *cat1 = caterva_empty_array(ctx, NULL, &pshape);
    caterva_from_buffer(cat1, &shape, buf_src);
    caterva_array_t *cat2 = caterva_empty_array_2(ctx, NULL, &pshape, &spshape);
    caterva_from_buffer_2(cat2, &shape, buf_src);

    caterva_dims_t start = caterva_new_dims(start_, ndim);
    caterva_dims_t stop = caterva_new_dims(stop_, ndim);

    uint64_t dest_size = 1;
    for (int i = 0; i < stop.ndim; ++i) {
        dest_size *= stop.dims[i] - start.dims[i];
    }
    double *dest_buf = (double *) malloc((size_t)dest_size * cat1->ctx->cparams.typesize);
    double *dest_buf2 = (double *) malloc((size_t)dest_size * cat1->ctx->cparams.typesize);
    int64_t pshape_dest_[CATERVA_MAXDIM];
    for (int i = 0; i < ndim; ++i) {
        pshape_dest_[i] = stop_[i] - start_[i];
    }
    caterva_dims_t pshape_dest = caterva_new_dims(pshape_dest_, ndim);

    double t1, t2;

    int niter = 10;
    blosc_set_timestamp(&last);
    for (int i = 0; i < niter; i++) {
        caterva_get_slice_buffer(dest_buf, cat1, &start, &stop, &pshape_dest);
    }
    blosc_set_timestamp(&current);
    t1 = blosc_elapsed_secs(last, current) / niter;
    printf("get_slice_buffer: %.4fs\n", t1);

    blosc_set_timestamp(&last);
    for (int i = 0; i < niter; i++) {
        caterva_get_slice_buffer_2(dest_buf2, cat2, &start, &stop, &pshape_dest);
    }
    blosc_set_timestamp(&current);
    t2 = blosc_elapsed_secs(last, current) / niter;
    printf("get_slice_buffer_2: %.4fs\n", t2);

    printf("Speed-up: %.4f\n", t1 / t2);

    for (int i = 0; i < dest_size; ++i) {
        if (dest_buf[i] != dest_buf2[i]) {
            return -1;
        }
    }

    free(buf_src);
    free(dest_buf);
    caterva_free_array(cat1);
    caterva_free_array(cat2);
    caterva_free_ctx(ctx);

    return 0;
}
