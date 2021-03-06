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

#include "test_common.h"

static void test_get_slice(caterva_ctx_t *ctx, int8_t ndim, int64_t *shape_, int64_t *pshape_,
                           int64_t *start_, int64_t *stop_, int64_t *pshape_dest_,
                           bool persist, bool copy, double *result) {

    caterva_dims_t shape = caterva_new_dims(shape_, ndim);
    caterva_dims_t start = caterva_new_dims(start_, ndim);
    caterva_dims_t stop = caterva_new_dims(stop_, ndim);

    caterva_array_t *src;
    blosc2_frame* frame = NULL;
    if (pshape_ != NULL) {
        if (persist) {
            frame = &(blosc2_frame) {.fname = "persistency.caterva"};
        }
        caterva_dims_t pshape = caterva_new_dims(pshape_, ndim);
        src = caterva_empty_array(ctx, frame, &pshape);
    } else {
        src = caterva_empty_array(ctx, NULL, NULL);
    }

    int64_t buf_size = 1;
    for (int i = 0; i < CATERVA_MAXDIM; ++i) {
        buf_size *= (shape.dims[i]);
    }

    double *buf_src = (double *) malloc((size_t)buf_size * src->ctx->cparams.typesize);
    for (int64_t i = 0; i < buf_size; ++i) {
        buf_src[i] = (double) i;
    }
    caterva_from_buffer(src, &shape, buf_src);

    caterva_array_t *dest;
    if (pshape_dest_ != NULL) {
        caterva_dims_t pshape_dest = caterva_new_dims(pshape_dest_, ndim);
        dest = caterva_empty_array(ctx, NULL, &pshape_dest);
    } else {
        dest = caterva_empty_array(ctx, NULL, NULL);
    }

    if (pshape_ != NULL) {
        if (persist) {
            // Close src and reopen the caterva frame persisted on-disk
            caterva_free_array(src);
            src = caterva_from_file(ctx, "persistency.caterva", copy);
        }
    }

    caterva_get_slice(dest, src, &start, &stop);
    caterva_squeeze(dest);
    double *buf_dest = (double *) malloc((size_t)dest->size * src->ctx->cparams.typesize);
    caterva_to_buffer(dest, buf_dest);

    assert_buf(buf_dest, result, (size_t)dest->size, 1e-14);
    free(buf_src);
    free(buf_dest);
    caterva_free_array(src);
    caterva_free_array(dest);
}

LWTEST_DATA(get_slice) {
    caterva_ctx_t *ctx;
};

LWTEST_SETUP(get_slice) {
    data->ctx = caterva_new_ctx(NULL, NULL, BLOSC2_CPARAMS_DEFAULTS, BLOSC2_DPARAMS_DEFAULTS);
    data->ctx->cparams.typesize = sizeof(double);
}

LWTEST_TEARDOWN(get_slice) {
    caterva_free_ctx(data->ctx);
}

LWTEST_FIXTURE(get_slice, ndim_2_plain) {
    const int8_t ndim = 2;
    int64_t shape_[] = {10, 10};
    int64_t start_[] = {5, 3};
    int64_t stop_[] = {9, 10};

    double result[1024] = {53, 54, 55, 56, 57, 58, 59, 63, 64, 65, 66, 67, 68, 69, 73, 74, 75, 76,
                           77, 78, 79, 83, 84, 85, 86, 87, 88, 89};

    test_get_slice(data->ctx, ndim, shape_, NULL, start_, stop_, NULL, false, false, result);
}

LWTEST_FIXTURE(get_slice, ndim_3) {
    const int8_t ndim = 3;
    int64_t shape_[] = {10, 10, 10};
    int64_t pshape_[] = {3, 5, 2};
    int64_t start_[] = {3, 0, 3};
    int64_t stop_[] = {6, 7, 10};
    int64_t pshape_dest_[] = {2, 4, 3};
    bool persist = true;
    bool copy = false;

    double result[1024] = {303, 304, 305, 306, 307, 308, 309, 313, 314, 315, 316, 317, 318, 319,
                           323, 324, 325, 326, 327, 328, 329, 333, 334, 335, 336, 337, 338, 339,
                           343, 344, 345, 346, 347, 348, 349, 353, 354, 355, 356, 357, 358, 359,
                           363, 364, 365, 366, 367, 368, 369, 403, 404, 405, 406, 407, 408, 409,
                           413, 414, 415, 416, 417, 418, 419, 423, 424, 425, 426, 427, 428, 429,
                           433, 434, 435, 436, 437, 438, 439, 443, 444, 445, 446, 447, 448, 449,
                           453, 454, 455, 456, 457, 458, 459, 463, 464, 465, 466, 467, 468, 469,
                           503, 504, 505, 506, 507, 508, 509, 513, 514, 515, 516, 517, 518, 519,
                           523, 524, 525, 526, 527, 528, 529, 533, 534, 535, 536, 537, 538, 539,
                           543, 544, 545, 546, 547, 548, 549, 553, 554, 555, 556, 557, 558, 559,
                           563, 564, 565, 566, 567, 568, 569};


    test_get_slice(data->ctx, ndim, shape_, pshape_, start_, stop_, pshape_dest_, persist, copy, result);
}

LWTEST_FIXTURE(get_slice, ndim_3_2) {
    const int8_t ndim = 3;
    int64_t shape_[] = {5, 6, 3};
    int64_t pshape_[] = {3, 3, 3};
    int64_t start_[] = {2, 1, 0};
    int64_t stop_[] = {4, 4, 2};
    int64_t pshape_dest_[] = {2, 3, 2};     // epshape ??????????????????????????????
    bool persist = true;
    bool copy = false;

    double result[1024] = {39, 40, 42, 43, 45, 46, 57, 58, 60, 61, 63, 64, 0, 0, 0, 0, 0, 0};


    test_get_slice(data->ctx, ndim, shape_, pshape_, start_, stop_, pshape_dest_, persist, copy, result);
}

LWTEST_FIXTURE(get_slice, ndim_4_plain) {
    const int8_t ndim = 4;
    int64_t shape_[] = {10, 10, 10, 10};
    int64_t start_[] = {5, 3, 9, 2};
    int64_t stop_[] = {9, 6, 10, 7};

    double result[1024] = {5392, 5393, 5394, 5395, 5396, 5492, 5493, 5494, 5495, 5496, 5592, 5593,
                           5594, 5595, 5596, 6392, 6393, 6394, 6395, 6396, 6492, 6493, 6494, 6495,
                           6496, 6592, 6593, 6594, 6595, 6596, 7392, 7393, 7394, 7395, 7396, 7492,
                           7493, 7494, 7495, 7496, 7592, 7593, 7594, 7595, 7596, 8392, 8393, 8394,
                           8395, 8396, 8492, 8493, 8494, 8495, 8496, 8592, 8593, 8594, 8595, 8596};

    test_get_slice(data->ctx, ndim, shape_, NULL, start_, stop_, NULL, false, false, result);
}

LWTEST_FIXTURE(get_slice, ndim_5) {
    const int8_t ndim = 5;
    int64_t shape_[] = {10, 10, 10, 10, 10};
    int64_t pshape_[] = {3, 5, 2, 4, 5};
    int64_t start_[] = {6, 0, 5, 5, 7};
    int64_t stop_[] = {8, 9, 6, 6, 10};
    int64_t pshape_dest_[] = {3, 5, 1, 1, 5};
    bool persist = true;
    bool copy = true;

    double result[1024] = {60557, 60558, 60559, 61557, 61558, 61559, 62557, 62558, 62559, 63557,
                           63558, 63559, 64557, 64558, 64559, 65557, 65558, 65559, 66557, 66558,
                           66559, 67557, 67558, 67559, 68557, 68558, 68559, 70557, 70558, 70559,
                           71557, 71558, 71559, 72557, 72558, 72559, 73557, 73558, 73559, 74557,
                           74558, 74559, 75557, 75558, 75559, 76557, 76558, 76559, 77557, 77558,
                           77559, 78557, 78558, 78559};

    test_get_slice(data->ctx, ndim, shape_, pshape_, start_, stop_, pshape_dest_, persist, copy, result);
}

LWTEST_FIXTURE(get_slice, ndim_6_plain) {
    const int8_t ndim = 6;
    int64_t shape_[] = {10, 10, 10, 10, 10, 10};
    int64_t start_[] = {0, 4, 2, 4, 5, 1};
    int64_t stop_[] = {1, 7, 4, 6, 8, 3};

    double result[1024] = {42451, 42452, 42461, 42462, 42471, 42472, 42551, 42552, 42561, 42562,
                           42571, 42572, 43451, 43452, 43461, 43462, 43471, 43472, 43551, 43552,
                           43561, 43562, 43571, 43572, 52451, 52452, 52461, 52462, 52471, 52472,
                           52551, 52552, 52561, 52562, 52571, 52572, 53451, 53452, 53461, 53462,
                           53471, 53472, 53551, 53552, 53561, 53562, 53571, 53572, 62451, 62452,
                           62461, 62462, 62471, 62472, 62551, 62552, 62561, 62562, 62571, 62572,
                           63451, 63452, 63461, 63462, 63471, 63472, 63551, 63552, 63561, 63562,
                           63571, 63572};

    test_get_slice(data->ctx, ndim, shape_, NULL, start_, stop_, NULL, false, false, result);
}

LWTEST_FIXTURE(get_slice, ndim_7) {
    const int8_t ndim = 7;
    int64_t shape_[] = {4, 5, 3, 4, 4, 5, 2};
    int64_t pshape_[] = {3, 4, 2, 2, 2, 3, 1};
    int64_t start_[] = {1, 4, 2, 2, 2, 2, 1};
    int64_t stop_[] = {4, 5, 3, 4, 3, 5, 2};
    int64_t pshape_dest_[] = {3, 1, 1, 2, 1, 3, 1};
    bool persist = true;
    bool copy = false;

    double result[1024] = {4745, 4747, 4749, 4785, 4787, 4789, 7145, 7147, 7149, 7185, 7187,
                         7189, 9545, 9547, 9549, 9585, 9587, 9589};

    test_get_slice(data->ctx, ndim, shape_, pshape_, start_, stop_, pshape_dest_, persist, copy, result);
}

LWTEST_FIXTURE(get_slice, ndim_8_plain) {
    const int8_t ndim = 8;
    int64_t shape_[] = {5, 3, 4, 5, 4, 3, 2, 3};
    int64_t start_[] = {3, 2, 2, 2, 2, 1, 1, 1};
    int64_t stop_[] = {4, 3, 3, 3, 3, 3, 2, 3};

    double result[1024] = {16750, 16751, 16756, 16757};

    test_get_slice(data->ctx, ndim, shape_, NULL, start_, stop_, NULL, false, false, result);
}
