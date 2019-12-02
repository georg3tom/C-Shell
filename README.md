# C-Shell

## Building from source

```
$ make
```

## RUN
```
$ ./a.out
```
Added func overkill, pinfo and kjob<br>

## REQ
main.c :main file<br>
def.c: func defintions and general funcs.<br>
ls.c: func def for ls implementation of ls.<br>
proc.c :fun def for pinfo and background check.<br>

1.gen_prompt() prints the prompt<br>
2.builtin func in main.c<br>
3.4.ls in ls.c using stat structure<br>
5.pinfo using /proc/pid/stat{,m}<br>
6.bg check using waitpid check<br>
7.history is saved to .hist file<br>
8.Redirection in redir.c<br>
9.Piping in pipe.c<br>
10.jobs,kjob,fg,bg,setenv,unsetenv,quit,overkill,ctrz,ctrlc are in their specific files

