# mccomb


### Command line options
* Argument options

| Option  |           Argument                    |                      Example usage              |   Use     |
|:--------|-------------------------------------- |-------------------------------------------------|-----------|
|-in      | (input filename)                      | `mccomb -in in.root`                            | mandatory |
|-out     | (output filename)                     | `mccomb -in in.root -out out.root`              | mandatory |
|-run     | (Run number for the dark file)        | `mccomb -in in.root -run XXXXXX`                | optional  |
|-seed    | (Random number seed)                  | `mccomb -in in.root -seed XXXXXX`               | optional  |
|-TMAX    | (Max. time range for merge data) [ns] | `mccomb -in in.root -TMAX 500.e3`               | optional  |
|-TCUT    | (offset time for merge data) [ns]     | `mccomb -in in.root -TCUT 18.e3`                | optional  |
|-TDEAD   | (dead time after pmt-hit) [ns]        | `mccomb -in in.root -TDEAD 900`                 | optional  |
