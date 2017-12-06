# defq
Ultra-fast Multi-threaded FASTQ Demultiplexing

# introduction
Use either index1 or index2 to demultiplex a single FASTQ file to multiple FASTQ files. This tool is developed in C++, with multi-threading supported.

# simple usage
```
# read1 and read2 are precessed separately
defq -i in.R1.fq -o demux_out_dir -s samplesheet.csv -f .R1.fq.gz
defq -i in.R2.fq -o demux_out_dir -s samplesheet.csv -f .R2.fq.gz
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

# sample sheet
```csv
#filename, #index1, #index2
file1, ATTCAGAA,
file2, ATTACTCG,
file3, GAGATTCC,
file4, CGCTCATT,
```
A sample sheet is a CSV file with 3 columns (filename, index1, index2). You can use index1 for all samples, or use index2 for all samples, but you cannot use both index1 and index2.