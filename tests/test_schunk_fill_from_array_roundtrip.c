#include "tests_common.h"

#define MAXDIM 8

int tests_run = 0;

int convert_to_array(char *line, size_t *shape)
{
    /* Convert string to an array */

    char *tok;
    tok = strtok(line, "-");
    int i = 0;
    while (tok != NULL)
    {
        sscanf(tok, "%lu", &shape[i]);
        tok = strtok(NULL, "-");
        i++;
    }
    return 0;
}

void get_fields(char *line, size_t *shape, size_t *cshape, int* dimensions)
{
    /* Get the fields of a csv line */

    char *shape_str;
    char *cshape_str;
    char *tok;
    char *tmp = line;

    tok = strtok(tmp, ";");
    shape_str = strdup(tok);
    tok = strtok(NULL, ";");
    cshape_str = strdup(tok);
    tok = strtok(NULL, ";");
    *dimensions = atoi(tok);

    convert_to_array(shape_str, shape);
    convert_to_array(cshape_str, cshape);
}

char* test_roundtrip(size_t shape[], size_t cshape[], int dimensions)
{
    /* Create dparams, cparams and pparams */

    blosc2_cparams cp = BLOSC_CPARAMS_DEFAULTS;
    cp.typesize = sizeof(double);
    cp.filters[BLOSC_MAX_FILTERS - 1] = BLOSC_SHUFFLE;

    blosc2_dparams dp = BLOSC_DPARAMS_DEFAULTS;

    caterva_pparams pp;
    for (int i = 0; i < MAXDIM; i++)
    {
        pp.shape[i] = shape[i];
        pp.cshape[i] = cshape[i];
    }
    pp.dimensions = dimensions;

    /* Create a caterva array */

    caterva_array *carr = caterva_new_array(cp, dp, pp);

    /* Create original data */

    double *arr = (double *)malloc(carr->size * sizeof(double));
    for (int i = 0; i < carr->size; i++)
    {
        arr[i] = (double)i;
    }
    
    /* Fill empty caterva_array with original data */

    caterva_schunk_fill_from_array(arr, carr);

    /* Fill dest array with caterva_array data */

    double *arr_dest = (double *)malloc(carr->size * sizeof(double));
    caterva_array_fill_from_schunk(carr, arr_dest);

    /* Testing */

    for(size_t i = 0; i < carr->size; i++)
    {
        mu_assert("ERROR. Original and resulting arrays are not equal!", arr[i] == arr_dest[i]);
    }
    
    /* Free mallocs */

    free(arr);
    free(arr_dest);
    caterva_free_array(carr);

    return 0;
}

static char* all_tests(size_t shape[], size_t cshape[], int* dimensions) {

    /* Read csv file */

    FILE *stream = fopen("test_schunk_fill_from_array_roundtrip.csv", "r");
    mu_assert("ERROR al abrir el fichero csv", stream != NULL);

    /* Run a test for each line of csv file */

    char line[1024];
    fgets(line, 1024, stream);
    while (fgets(line, 1024, stream))
    {
        char *tmp = line;
        get_fields(tmp, shape, cshape, dimensions);
        mu_run_test(test_roundtrip(shape, cshape, *dimensions));
    }
    return 0;
}

int main()
{
    /* Set stream buffer */

    setbuf(stdout, NULL);
    
    /* Define data needed for run a test */

    size_t shape[MAXDIM];
    size_t cshape[MAXDIM];
    int dimensions;

    /* Print test result */

    char* result = all_tests(shape, cshape, &dimensions);

    if (result != 0) {
        printf(" (%s)", result);
    }
    else {
        printf(" ALL TESTS PASSED");
    }
    printf("\tTests run: %d\n", tests_run);

    return 0;
}
