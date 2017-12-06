# defq
Ultra-fast Multi-threaded FASTQ Demultiplexing

# introduction
Use either index1 or index2 to demultiplex a single FASTQ file to multiple FASTQ files. This tool is developed in C++ with multi-threading supported.

# simple usage
```
# read1 and read2 are precessed separately
defq -i in.R1.fq -o demux_out_dir -s samplesheet.csv -f .R1.fq
defq -i in.R2.fq -o demux_out_dir -s samplesheet.csv -f .R2.fq
```
Where samplesheet.csv is 3-column (filename, index1, index2) CSV file. You can use index1 for all samples, or use index2 for all samples, but you cannot use both index1 and index2. An example:
```csv
#filename, #index1, #index2
file1, ATTCAGAA,
file2, ATTACTCG,
file3, GAGATTCC,
file4, CGCTCATT,
```
You will then have following output:
```
demux_out_dir/
├── Undetermined.R1.fq
├── Undetermined.R2.fq
├── file1.R1.fq
├── file1.R2.fq
├── file2.R1.fq
├── file2.R2.fq
├── file3.R1.fq
├── file3.R2.fq
├── file4.R1.fq
└── file4.R2.fq
```

# get defq
## compile from source
```shell
# get source (you can also use browser to download from master or releases)
git clone https://github.com/OpenGene/defq.git

# build
cd defq
make

# install
sudo make install
```

# all options
```
usage: ./defq --in1=string --sample_sheet=string [options] ... 
options:
  -i, --in1             input file name (string)
  -s, --sample_sheet    a CSV file contains three columns (filename, index1, index2) (string)
  -o, --out_folder      output folder, default is current working directory (string [=.])
  -f, --suffix1         the suffix to be appended to the output file name given in sample sheet, default is none (string [=])
  -u, --undetermined    the file name of undetermined data, default is Undetermined (string [=Undetermined])
  -z, --compression     compression level for gzip output (1 ~ 9). 1 is fastest, 9 is smallest, default is 2. (int [=2])
  -?, --help            print this message
```
