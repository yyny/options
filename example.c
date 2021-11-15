#include <stdio.h>

#include "options.h"

struct option options[];

static int help(char *topic, void *userdata)
{
    (void)userdata;

    if (topic == NULL) {
        puts("Usage: prog [options...] [FILES...]");
        puts("Options:");
        for (struct option *o = options; o->pattern; o++) {
            if (o->name && o->userdata) {
                printf("  %-*s%s\n", 32, o->name, (char *)o->userdata);
            }
        }
    } else {
        printf("Help topic: %s\n", topic);
    }

    return OPT_EXIT;
}
static int version(char *arg, void *userdata)
{
    (void)arg;
    (void)userdata;

    puts("prog 1.0.0");
    puts("Copyright (c) 2016 Jonne Ransijn");

    return OPT_EXIT;
}
static int color(char *color, void *userdata)
{
    (void)userdata;

    printf("color: %s", color);

    return OPT_OK;
}
static int add_input_file(char *file_name, void *userdata)
{
    (void)userdata;

    printf("<%s\n", file_name);

    return OPT_OK;
}
static int add_output_file(char *file_name, void *userdata)
{
    (void)userdata;

    printf(">%s\n", file_name);

    return OPT_OK;
}
static int add_stdin(char *arg, void *userdata)
{
    (void)arg;
    (void)userdata;

    printf("<(stdin)\n");

    return OPT_OK;
}
static int invalid_option(char *option, void *userdata)
{
    (void)userdata;

    printf("prog: error: unrecognized command line option '%s'\n", option);

    return OPT_EXIT;
}

struct option options[] = {
    { "--help[=topic]",     "--help?",    help,               "Display help information" },
    { "--version",          "--version",  version,            "Display version information" },
    { "--color<=what>",     "--color=",   color,              "Configure terminal colors" },
    { NULL,                 "-- ",        add_input_file,     NULL },
    { NULL,                 "-",          add_stdin,          NULL },
    { "-o <file>",          "-o.",        add_output_file,    "Add output file" },
    { "--output <file>",    "--output:",   add_output_file,    NULL },
    { NULL,                 "-*",         invalid_option,     NULL },
    { NULL,                 "*",          add_input_file,     NULL },
    { NULL,                 NULL,         NULL,               NULL },
};

int main(int argc, char *argv[])
{
    if (parse_options(options, NULL, argc, argv) == OPT_EXIT) return 0;

    return 0;
}
