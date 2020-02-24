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
    int8_t typesize = sizeof(double);

    // Create a context
    caterva_ctx_t *ctx = caterva_new_ctx(NULL, NULL, BLOSC2_CPARAMS_DEFAULTS, BLOSC2_DPARAMS_DEFAULTS);
    ctx->cparams.typesize = sizeof(double);
    ctx->cparams.compcode = BLOSC_LZ4;

    // Define the partition shape for the first array
    const int8_t ndim = 3;
    int64_t shape_[] = {252, 252, 252};
    int64_t pshape_[] = {64, 64, 64};
    int64_t spshape_[] = {15, 15, 15};

    int64_t buf_size = 1;
    for (int i = 0; i < ndim; ++i) {
        buf_size *= (shape_[i]);
    }

    double *buf_src = (double *) malloc((size_t)buf_size * typesize);
    for (int64_t i = 0; i < buf_size; ++i) {
        buf_src[i] = (double) i;
    }

    blosc_timestamp_t last, current;
    caterva_dims_t shape = caterva_new_dims(shape_, ndim);
    caterva_dims_t pshape = caterva_new_dims(pshape_, ndim);
    caterva_dims_t spshape = caterva_new_dims(spshape_, ndim);

    caterva_array_t *src = caterva_empty_array_2(ctx, NULL, &pshape, &spshape);
    caterva_from_buffer_2(src, &shape, buf_src);

    double *buf_dest = (double *) malloc((size_t)buf_size * src->ctx->cparams.typesize);

    double tt;

    int niter = 10;
    blosc_set_timestamp(&last);
    for (int i = 0; i < niter; i++) {
        caterva_to_buffer_2(src, buf_dest);
    }
    blosc_set_timestamp(&current);
    tt = blosc_elapsed_secs(last, current);
    printf("to_buffer_2: %.4fs\n", tt / niter);


    blosc_set_timestamp(&last);
    for (int i = 0; i < niter; i++) {
        caterva_to_buffer_3(src, buf_dest);
    }
    blosc_set_timestamp(&current);
    tt = blosc_elapsed_secs(last, current);
    printf("to_buffer_3: %.4fs\n", tt / niter);

    caterva_array_t *src2 = caterva_empty_array(ctx, NULL, &pshape);
    caterva_from_buffer(src2, &shape, buf_src);

    blosc_set_timestamp(&last);
    for (int i = 0; i < niter; i++) {
        caterva_to_buffer(src, buf_dest);
    }
    blosc_set_timestamp(&current);
    tt = blosc_elapsed_secs(last, current);
    printf("to_buffer_o: %.4fs\n", tt / niter);


    free(buf_src);
    free(buf_dest);
    caterva_free_array(src);
    caterva_free_array(src2);

    return 0;
}
