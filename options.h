#ifndef OPTIONS_H
#define OPTIONS_H

#define OPT_MORE (1)
#define OPT_OK (0)
#define OPT_EXIT (-1)
#define OPT_ERROR (-2)

typedef int (*option_cb)(char *, void *);

struct option {
    char *name;
    char *pattern;
    option_cb callback;
    void *userdata;
};

/*
PATTERN CHARACTERS

    '\0' EOF
    '$'  EOF
    '*'  anything
    ','  anything
    ' '  EOF
    '.'  EOF or anything else (or ' ')
    '='  '='
    '?'  '=' or EOF (or ' ')
    ':'  '=' or EOF

PARAMETERIZED BY

    '\0' the entire argument
    '$'  the entire argument
    '*'  the entire argument
    ','  the string after the pattern (or NULL)
    ' '  the next argument (or NULL)
    '.'  the string after the pattern or the next argument (or NULL)
    '='  the string after the '='
    '?'  the string after the '=' (or NULL)
    ':'  the string after the '=' or the next argument (or NULL)

SUMMARY

    pat | description                              pattern         example match                                   resulting argument
    ----|-----------------------------------------------------------------------------------------------------------------------------
    '$' | no argument                              "--help$"       `--help`                                        (gives: "--help")
    '*' | no argument                              "-Wl,*"         `-Wl,--defsym`                                  (gives: "--defsym")
    ',' | no argument                              "-W,"           `-Werror`                                       (gives: "error")
    ' ' | optional argument after ' '              "-- "           `--` or `-- input.txt`                          (gives: "input.txt" or NULL)
    '.' | optional argument after ' ' or ''        "-o."           `-o output.txt` or `-ooutput.txt`               (gives: "output.txt")
    '=' | REQUIRED argument after '='              "--color="      `--color=auto`                                  (gives: "auto")
    '?' | optional argument after '='              "--help?"       `--help=topic` or `--help`                      (gives: "topic" or NULL)
    ':' | optional argument after '=' or ' '       "--output:"     `--output=output.txt` or `--output output.txt`  (gives: "output.txt")

NOTE

    patterns that match ' ' will only ever give NULL when found at the end of the argument list.
*/

static inline int parse_options(struct option *options, void *data, int argc, char *argv[])
{
    for (int i=1; i < argc; i++) {
        char *arg = argv[i];
        for (struct option *o = options; o->pattern; o++) {
            char *pattern = o->pattern;
            option_cb cb = o->callback;
            for (size_t j=0; ; j++) {
                int status;
                int b = pattern[j] && !pattern[j+1], c = pattern[j], d = arg[j];
                if      (b && c == '$' && !d) /* always succeed */;
                else if (b && c == '$') break;
                else if (b && c == '*') /* always succeed */;
                else if (b && c == ',') arg = d ? arg + j : NULL;
                else if (b && c == ' ' && !d) arg = i < argc ? argv[++i] : NULL;
                else if (b && c == '.') arg = d ? arg + j : i < argc ? argv[++i] : NULL;
                else if (b && c == '=' && d == '=') arg = arg + j + 1;
                else if (b && c == '=') break;
                else if (b && c == '?' && (d == '=' || !d)) arg = d == '=' ? arg + j + 1 : NULL;
                else if (b && c == '?') break;
                else if (b && c == ':' && (d == '=' || !d)) arg = d == '=' ? arg + j + 1 : (i < argc ? argv[++i] : NULL);
                else if (b && c == ':') break;
                else if (c != d) break;
                else if (!c);
                else continue;
                for (;;) {
                    status = cb(arg, data);
                    if (status != OPT_MORE || i >= argc) break;
                    arg = argv[++i];
                }
                if (status < 0) return status;
                goto next;

            }
        }
        // I could restructure the program's flow
        // Or use one little 'goto' instead
        // Eh, screw good practice.
        // How bad can it be?
        //                                                   .--.__
        //                                                 .~ (@)  ~~~---_
        //                                                {     `-_~,,,,,,)
        //                                                {    (_  ',
        //                                                 ~    . = _',
        //                                                  ~-   '.  =-'
        //                                                    ~     :
        // .                                             _,.-~     ('');
        // '.                                         .-~        \  \ ;
        //   ':-_                                _.--~            \  \;      _-=,.
        //     ~-:-.__                       _.-~                 {  '---- _'-=,.
        //        ~-._~--._             __.-~                     ~---------=,.`
        //            ~~-._~~-----~~~~~~       .+++~~~~~~~~-__   /
        //                 ~-.,____           {   -     +   }  _/
        //                         ~~-.______{_    _ -=\ / /_.~
        //                              :      ~--~    // /         ..-
        //                              :   / /      // /         ((
        //                              :  / /      {   `-------,. ))
        //                              :   /        ''=--------. }o
        //                 .=._________,'  )                     ))
        //                 )  _________ -''                     ~~
        //                / /  _ _
        //               (_.-.'O'-'.
next:;
    }

    return OPT_OK;
}

#endif /* OPTIONS_H */
