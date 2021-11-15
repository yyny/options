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

// the last character of the pattern matches:
// ' '  the end of the argument
// '?'  the end of the argument or '='
// '='  the end of the argument or '='
// '.'  the end of the argument or anything else
// ':'  the end of the argument or '=' or anything else
// ','  any nonempty string of characters
// ';'  '=' or anything else
// '*'  any string of characters
// '$'  the end of the argument
// ...  the character itself
// arg is parameterized by:
// ' '  the next argument (or NULL)
// '?'  the string after the '=' or the next argument (or NULL)
// '='  the string after the '=' (or NULL)
// '.'  the string after the pattern or the next argument (or NULL)
// ':'  the string after the '=' or after the pattern or the next argument (or NULL)
// ','  the string after the pattern (or NULL)
// ';'  the string after the '=' or after the pattern (or NULL)
// '*'  the entire argument
// '$'  the entire argument
// ...  the entire argument
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
                if (b && c == ' ' && !d) arg = i < argc ? argv[++i] : NULL;
                else if (b && c == '?' && (d == '=' || !d)) arg = d == '=' ? arg + j + 1 : (i < argc ? argv[++i] : NULL);
                else if (b && c == '=' && (d == '=' || !d)) arg = d == '=' ? arg + j + 1 : NULL;
                else if (b && c == '.') arg = d ? arg + j : i < argc ? argv[++i] : NULL;
                else if (b && c == ':') arg = d ? arg + j + (d == '=') : (i < argc ? argv[++i] : NULL);
                else if (b && c == ',') arg = d ? arg + j : NULL;
                else if (b && c == ';') arg = d ? arg + j + (d == '=') : NULL;
                else if (b && c == '*');
                else if (b && c == '$' && !d);
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
