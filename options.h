#ifndef OPTIONS_H
#define OPTIONS_H

#define OPT_OK 0
#define OPT_EXIT -1

typedef int (*option_cb)(char *, void *);

struct option {
    char *name;
    char *pattern;
    option_cb callback;
    char *help;
};

static inline int parse_options(struct option *options, void *data, int argc, char *argv[])
{
    for (size_t i=1; i < argc; i++) {
        char *arg = argv[i];
        for (struct option *o = options; o->pattern; o++) {
            char *pattern = o->pattern;
            option_cb cb = o->callback;
            for (size_t j=0; ; j++) {
                int status;
                char c = pattern[j], d = arg[j];
                if (c == ' ' && !d) status = cb(i < argc ? argv[++i] : NULL, data);
                else if (c == '=' && (d == '=' || !d)) status = cb(d == '=' ? arg + j + 1 : NULL, data);
                else if (c == '.') status = cb(d ? arg + j : i < argc ? argv[++i] : NULL, data);
                else if (c == ':') status = cb(d ? arg + j + (d == '=') : i < argc ? argv[++i] : NULL, data);
                else if (c == '*') status = cb(arg, data);
                else if (c != d) break;
                else if (!c) status = cb(arg, data);
                else continue;
                if (status == OPT_EXIT) return OPT_EXIT;
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
