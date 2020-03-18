# depile
Unstacking stored informations in text files for extracting data

```
$ depile
Usage: depile [OPTION] [KEYWORDS] [-f FILE1] [-f FILE2] ...
Arguments:
	-f <filename>
		Read file (and not stdin by default)
	-t
		Add the current date and time in the first column of data
```

Example:
```
$ cat test.dat
X: 101
Y: 102
Z: 103
X: 201 Y: 202 Z: 203
Z: 303 Y: 302
X: 301

$ depile X: Y: Z: -f test.dat
101	102	103
201	202	203
301	302	303
```

A real case: extracting the force of received wireless AP around you.
```
$ iwlist wlan0 scanning | depile 'ESSID:' 'Signal level=' 'Frequency:'
```
If used with watch or a crontab you can use "-t" to record time, too.
