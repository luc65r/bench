#include <argp.h>
#include <assert.h>
#include <error.h>
#include <regex.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bench.h"

const char *argp_program_version = "bench 0.1";
const char *argp_program_bug_address = "<lucas@ransan.tk>";

static char doc[] = "bench -- benchmark functions";
static char args_doc[] = "FILES...";

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
static struct argp_option options[] = {
    { 0, 0, 0, 0, "Pattern options" },
    { "regexp", 'e', "PATTERN", 0, "use PATTERN for matching functions" },
    { "extended-regexp", 'E', 0, 0, "PATTERN is an extended regular expression" },
    { "ignore-case", 'i', 0, 0, "do not differentiate case" },

    { 0 }
};
#pragma GCC diagnostic pop

struct arguments {
    char **files;
    char *pattern;
    bool extended_regexp;
    bool ignore_case;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
    case 'e':
        arguments->pattern = arg;
        break;

    case 'E':
        arguments->extended_regexp = true;
        break;

    case 'i':
        arguments->ignore_case = true;
        break;

    case ARGP_KEY_ARG:
        arguments->files = &state->argv[state->next - 1];
        state->next = state->argc;
        break;

    case ARGP_KEY_NO_ARGS:
        argp_usage(state);
        /* argp_usage calls exit */
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
static struct argp argp = { options, parse_opt, args_doc, doc };
#pragma GCC diagnostic pop

int main(int argc, char **argv) {
    struct arguments arguments = {
        .files = NULL,
        .pattern = ".*",
        .extended_regexp = false,
        .ignore_case = false,
    };

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    assert(arguments.files != NULL);
    assert(arguments.pattern != NULL);

    regex_t re;
    int err = regcomp(&re, arguments.pattern,
                      REG_NOSUB
                      | (arguments.extended_regexp ? REG_EXTENDED : 0)
                      | (arguments.ignore_case ? REG_ICASE : 0));
    if (err != 0) {
        size_t errbuf_size = regerror(err, &re, NULL, 0);
        char *errbuf = malloc(errbuf_size);
        assert(errbuf != NULL);
        regerror(err, &re , errbuf, errbuf_size);
        error(1, 0, "error while compiling regex: %s", errbuf);
    }

    bench(arguments.files, &re);

    regfree(&re);

    return 0;
}
