/*
 * Copyright (c) 2018 Francesc Alted, Aleix Alcacer.
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
                           int64_t *start_, int64_t *stop_, int64_t *pshape_dest_, double *result) {

    caterva_dims_t shape = caterva_new_dims(shape_, ndim);
    caterva_dims_t start = caterva_new_dims(start_, ndim);
    caterva_dims_t stop = caterva_new_dims(stop_, ndim);

    caterva_array_t *src;
    if (pshape_ != NULL) {
        caterva_dims_t pshape = caterva_new_dims(pshape_, ndim);
        src = caterva_empty_array(ctx, NULL, &pshape);
    } else {
        src = caterva_empty_array(ctx, NULL, NULL);
    }

    int64_t buf_size = 1;
    for (int i = 0; i < CATERVA_MAXDIM; ++i) {
        buf_size *= (shape.dims[i]);
    }

    double *buf_src = (double *) malloc((size_t)buf_size * src->ctx->cparams.typesize);
    for (int i = 0; i < buf_size; ++i) {
        buf_src[i] = (double) i;
    }
    caterva_from_buffer(src, &shape, buf_src);

    uint64_t dest_size = 1;
    for (int i = 0; i < stop.ndim; ++i) {
        dest_size *= stop.dims[i] - start.dims[i];
    }

    double *dest_buf = (double *) malloc((size_t)dest_size * src->ctx->cparams.typesize);

    caterva_dims_t pshape_dest = caterva_new_dims(pshape_dest_, ndim);
    caterva_get_slice_buffer(dest_buf, src, &start, &stop, &pshape_dest);

    assert_buf(dest_buf, result, (size_t)dest_size, 1e-14);
    free(buf_src);
    free(dest_buf);
    caterva_free_array(src);

}

LWTEST_DATA(get_slice_buffer) {
    caterva_ctx_t *ctx;
};

LWTEST_SETUP(get_slice_buffer) {
    data->ctx = caterva_new_ctx(NULL, NULL, BLOSC2_CPARAMS_DEFAULTS, BLOSC2_DPARAMS_DEFAULTS);
    data->ctx->cparams.typesize = sizeof(double);
}

LWTEST_TEARDOWN(get_slice_buffer) {
    caterva_free_ctx(data->ctx);
}

LWTEST_FIXTURE(get_slice_buffer, ndim_2) {
    const int8_t ndim = 2;
    int64_t shape_[] = {10, 10};
    int64_t pshape_[] = {2, 3};
    int64_t start_[] = {5, 3};
    int64_t stop_[] = {9, 10};
    int64_t pshape_dest_[] = {0, 0};

    for (int i = 0; i < ndim; ++i) {
        pshape_dest_[i] = stop_[i] - start_[i];
    }

    double result[1024] = {53, 54, 55, 56, 57, 58, 59, 63, 64, 65, 66, 67, 68, 69, 73, 74, 75, 76,
                           77, 78, 79, 83, 84, 85, 86, 87, 88, 89};

    test_get_slice(data->ctx, ndim, shape_, pshape_, start_, stop_, pshape_dest_, result);
}

LWTEST_FIXTURE(get_slice_buffer, ndim_3_plain) {
    const int8_t ndim = 3;
    int64_t shape_[] = {10, 10, 10};
    int64_t start_[] = {3, 0, 3};
    int64_t stop_[] = {6, 7, 10};
    int64_t pshape_dest_[] = {0, 0, 0};

    for (int i = 0; i < ndim; ++i) {
        pshape_dest_[i] = stop_[i] - start_[i];
    }

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

    test_get_slice(data->ctx, ndim, shape_, NULL, start_, stop_, pshape_dest_, result);
}

LWTEST_FIXTURE(get_slice_buffer, ndim_4) {
    const int8_t ndim = 4;
    int64_t shape_[] = {10, 10, 10, 10};
    int64_t pshape_[] = {3, 2, 3, 2};
    int64_t start_[] = {5, 3, 9, 2};
    int64_t stop_[] = {9, 6, 10, 7};
    int64_t pshape_dest_[] = {0, 0, 0, 0};

    for (int i = 0; i < ndim; ++i) {
        pshape_dest_[i] = stop_[i] - start_[i];
    }

    double result[1024] = {5392, 5393, 5394, 5395, 5396, 5492, 5493, 5494, 5495, 5496, 5592, 5593,
                           5594, 5595, 5596, 6392, 6393, 6394, 6395, 6396, 6492, 6493, 6494, 6495,
                           6496, 6592, 6593, 6594, 6595, 6596, 7392, 7393, 7394, 7395, 7396, 7492,
                           7493, 7494, 7495, 7496, 7592, 7593, 7594, 7595, 7596, 8392, 8393, 8394,
                           8395, 8396, 8492, 8493, 8494, 8495, 8496, 8592, 8593, 8594, 8595, 8596};

    test_get_slice(data->ctx, ndim, shape_, pshape_, start_, stop_, pshape_dest_, result);
}

LWTEST_FIXTURE(get_slice_buffer, ndim_5_plain) {
    const int8_t ndim = 5;
    int64_t shape_[] = {10, 10, 10, 10, 10};
    int64_t start_[] = {6, 0, 5, 5, 7};
    int64_t stop_[] = {8, 9, 6, 6, 10};
    int64_t pshape_dest_[] = {0, 0, 0, 0, 0};

    for (int i = 0; i < ndim; ++i) {
        pshape_dest_[i] = stop_[i] - start_[i];
    }
    double result[1024] = {60557, 60558, 60559, 61557, 61558, 61559, 62557, 62558, 62559, 63557,
                           63558, 63559, 64557, 64558, 64559, 65557, 65558, 65559, 66557, 66558,
                           66559, 67557, 67558, 67559, 68557, 68558, 68559, 70557, 70558, 70559,
                           71557, 71558, 71559, 72557, 72558, 72559, 73557, 73558, 73559, 74557,
                           74558, 74559, 75557, 75558, 75559, 76557, 76558, 76559, 77557, 77558,
                           77559, 78557, 78558, 78559};

    test_get_slice(data->ctx, ndim, shape_, NULL, start_, stop_, pshape_dest_, result);
}

LWTEST_FIXTURE(get_slice_buffer, ndim_6) {
    const int8_t ndim = 6;
    int64_t shape_[] = {10, 10, 10, 10, 10, 10};
    int64_t pshape_[] = {6, 5, 3, 5, 4, 2};
    int64_t start_[] = {0, 4, 2, 4, 5, 1};
    int64_t stop_[] = {1, 7, 4, 6, 8, 3};
    int64_t pshape_dest_[] = {0, 0, 0, 0, 0, 0};

    for (int i = 0; i < ndim; ++i) {
        pshape_dest_[i] = stop_[i] - start_[i];
    }

    double result[1024] = {42451, 42452, 42461, 42462, 42471, 42472, 42551, 42552, 42561, 42562,
                           42571, 42572, 43451, 43452, 43461, 43462, 43471, 43472, 43551, 43552,
                           43561, 43562, 43571, 43572, 52451, 52452, 52461, 52462, 52471, 52472,
                           52551, 52552, 52561, 52562, 52571, 52572, 53451, 53452, 53461, 53462,
                           53471, 53472, 53551, 53552, 53561, 53562, 53571, 53572, 62451, 62452,
                           62461, 62462, 62471, 62472, 62551, 62552, 62561, 62562, 62571, 62572,
                           63451, 63452, 63461, 63462, 63471, 63472, 63551, 63552, 63561, 63562,
                           63571, 63572};

    test_get_slice(data->ctx, ndim, shape_, pshape_, start_, stop_, pshape_dest_, result);
}
/*
LWTEST_FIXTURE(get_slice_buffer, ndim_7_plain) {
    const int8_t ndim = 7;
    int64_t shape_[] = {10, 10, 10, 10, 10, 10, 10};
    int64_t start_[] = {5, 4, 3, 8, 4, 5, 1};
    int64_t stop_[] = {8, 6, 5, 9, 7, 7, 3};
    int64_t pshape_dest_[] = {0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < ndim; ++i) {
        pshape_dest_[i] = stop_[i] - start_[i];
    }
    double result[1024] = {5438451, 5438452, 5438461, 5438462, 5438551, 5438552, 5438561, 5438562,
                           5438651, 5438652, 5438661, 5438662, 5448451, 5448452, 5448461, 5448462,
                           5448551, 5448552, 5448561, 5448562, 5448651, 5448652, 5448661, 5448662,
                           5538451, 5538452, 5538461, 5538462, 5538551, 5538552, 5538561, 5538562,
                           5538651, 5538652, 5538661, 5538662, 5548451, 5548452, 5548461, 5548462,
                           5548551, 5548552, 5548561, 5548562, 5548651, 5548652, 5548661, 5548662,
                           6438451, 6438452, 6438461, 6438462, 6438551, 6438552, 6438561, 6438562,
                           6438651, 6438652, 6438661, 6438662, 6448451, 6448452, 6448461, 6448462,
                           6448551, 6448552, 6448561, 6448562, 6448651, 6448652, 6448661, 6448662,
                           6538451, 6538452, 6538461, 6538462, 6538551, 6538552, 6538561, 6538562,
                           6538651, 6538652, 6538661, 6538662, 6548451, 6548452, 6548461, 6548462,
                           6548551, 6548552, 6548561, 6548562, 6548651, 6548652, 6548661, 6548662,
                           7438451, 7438452, 7438461, 7438462, 7438551, 7438552, 7438561, 7438562,
                           7438651, 7438652, 7438661, 7438662, 7448451, 7448452, 7448461, 7448462,
                           7448551, 7448552, 7448561, 7448562, 7448651, 7448652, 7448661, 7448662,
                           7538451, 7538452, 7538461, 7538462, 7538551, 7538552, 7538561, 7538562,
                           7538651, 7538652, 7538661, 7538662, 7548451, 7548452, 7548461, 7548462,
                           7548551, 7548552, 7548561, 7548562, 7548651, 7548652, 7548661, 7548662};

    test_get_slice(data->ctx, ndim, shape_, NULL, start_, stop_, pshape_dest_, result);
}

LWTEST_FIXTURE(get_slice_buffer, ndim_8) {
    const int8_t ndim = 8;
    int64_t shape_[] = {10, 10, 10, 10, 10, 10, 10, 10};
    int64_t pshape_[] = {2, 6, 4, 3, 5, 3, 2, 4};
    int64_t start_[] = {3, 5, 2, 4, 5, 1, 6, 0};
    int64_t stop_[] = {6, 6, 4, 6, 7, 3, 7, 3};
    int64_t pshape_dest_[] = {0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < ndim; ++i) {
        pshape_dest_[i] = stop_[i] - start_[i];
    }

    double result[1024] = {35245160, 35245161, 35245162, 35245260, 35245261, 35245262, 35246160,
                           35246161, 35246162, 35246260, 35246261, 35246262, 35255160, 35255161,
                           35255162, 35255260, 35255261, 35255262, 35256160, 35256161, 35256162,
                           35256260, 35256261, 35256262, 35345160, 35345161, 35345162, 35345260,
                           35345261, 35345262, 35346160, 35346161, 35346162, 35346260, 35346261,
                           35346262, 35355160, 35355161, 35355162, 35355260, 35355261, 35355262,
                           35356160, 35356161, 35356162, 35356260, 35356261, 35356262, 45245160,
                           45245161, 45245162, 45245260, 45245261, 45245262, 45246160, 45246161,
                           45246162, 45246260, 45246261, 45246262, 45255160, 45255161, 45255162,
                           45255260, 45255261, 45255262, 45256160, 45256161, 45256162, 45256260,
                           45256261, 45256262, 45345160, 45345161, 45345162, 45345260, 45345261,
                           45345262, 45346160, 45346161, 45346162, 45346260, 45346261, 45346262,
                           45355160, 45355161, 45355162, 45355260, 45355261, 45355262, 45356160,
                           45356161, 45356162, 45356260, 45356261, 45356262, 55245160, 55245161,
                           55245162, 55245260, 55245261, 55245262, 55246160, 55246161, 55246162,
                           55246260, 55246261, 55246262, 55255160, 55255161, 55255162, 55255260,
                           55255261, 55255262, 55256160, 55256161, 55256162, 55256260, 55256261,
                           55256262, 55345160, 55345161, 55345162, 55345260, 55345261, 55345262,
                           55346160, 55346161, 55346162, 55346260, 55346261, 55346262, 55355160,
                           55355161, 55355162, 55355260, 55355261, 55355262, 55356160, 55356161,
                           55356162, 55356260, 55356261, 55356262};

    test_get_slice(data->ctx, ndim, shape_, pshape_, start_, stop_, pshape_dest_, result);
}
*/