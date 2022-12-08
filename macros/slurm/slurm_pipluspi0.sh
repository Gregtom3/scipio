#!/bin/bash
USERNAME="$USER"
hl="--------------------------------------------------------------------------------------------------------------------------------------------"
ana="nSidis" # either MC or nSidis
preprocess="catboost" # catboost or particleNet
volatiledir=/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML
pathtoscipio=/work/clas12/users/gmat/scipio
# ----------------------------------------------------------------------------------------------
# Create log and slurm dir for execution
logdir=""
slurmdir=""
NOW=$( date '+%F_%H_%M' )
NOWdir=/farm_out/gmat/clas12analysis.sidis.data/rga/scipio/$NOW
if [ -d "${NOWdir}/" ]; then
    rm -r ${NOWdir}
fi
mkdir ${NOWdir}
logdir=$NOWdir"/log"
slurmdir=$NOWdir"/slurm"
if [ -d "${logdir}/" ]; then
    rm -r ${logdir}
fi
if [ -d "${slurmdir}/" ]; then
    rm -r ${slurmdir}
fi
mkdir ${logdir}
mkdir ${slurmdir}

# ----------------------------------------------------------------------------------------------
# CPU and Memory Parameters
nCPUs=1
memPerCPU=2000
# ----------------------------------------------------------------------------------------------

# Create list of input files
input_files=$volatiledir"/postprocess_$preprocess/*$ana*.root"

# ----------------------------------------------------------------------------------------------

# Open the json file
json_file="../../utils/subdata.json"

# read in the json file
json_string=$(cat $json_file)

# Get all headings
headings=$(echo $json_string | sed 's/{//g' | sed 's/}//g' | sed 's/://g' | sed 's/\["[^]]*"\]//g' | sed 's/"//g' | sed 's/,//g' | sed 's/Energy//g')

# Setting beam energy of processing script

#energy=0
#if [ $subdata != "all" ]; then
    # extract the runs from the json string
#    runs_string=$(echo $json_string | grep -Po "$subdata.*?Runs\K[^]]+")

    # remove the brackets and whitespace 
#    runs=$(echo $runs_string | tr -d "][" | tr -d ' ' | tr -d '"' | tr -d ':')

    # split the runs string into an array
#    IFS=', ' read -r -a runs_array <<< "$runs"

    # get the energy
#    energy=$(echo $json_string | grep -Po "$subdata.*?Energy\K[^,}]+")
#fi


# ----------------------------------------------------------------------------------------------


for infile in $input_files 
do
    # Extract last part of the file name
    FILENAME=${infile##*/}
    RUNNAME=""
    if [[ $FILENAME == *"MC_"* ]]; then
        RUNNAME=`echo $FILENAME | cut -d "_" -f2-3 | cut -d "." -f1` 
    else
        RUNNAME=`echo $FILENAME | cut -d "_" -f2 | cut -d "." -f1` 
    fi
    
    # Search the JSON to find the correct energy
    # Loop through all headings
    for heading in $headings
    do
        if [[ ! $heading =~ "bending" ]]; then
          continue
        fi
        # Get the Runs list
        runs_string=$(echo $json_string | grep -Po "$heading.*?Runs\K[^]]+")
        # remove the brackets and whitespace 
        runs=$(echo $runs_string | tr -d "][" | tr -d ' ' | tr -d '"' | tr -d ':' | tr ',' ' ')
        # Loop through each Run in the list
        for run_number in $runs
        do
            # If the Run matches the Run given, print the energy
            if [ "$RUNNAME" = "$run_number" ]; then
                # get the energy
                energy=$(echo $json_string | grep -Po "$heading.*?Energy\K[^,}]+")
                energy=${energy//[: ]/}
            fi
        done
    done
    
    outfile="${infile//postprocess/pipluspi0}"
    
    slurmshell=${slurmdir}"/${ana}_${RUNNAME}_${preprocess}.sh"
    slurmslurm=${slurmdir}"/${ana}_${RUNNAME}_${preprocess}.slurm"

    touch $slurmshell
    touch $slurmslurm
    chmod +x $slurmshell

    cat >> $slurmslurm <<EOF
#!/bin/bash
#SBATCH --account=clas12
#SBATCH --partition=production
#SBATCH --mem-per-cpu=${memPerCPU}
#SBATCH --job-name=job_${ana}_${runNumber}_${preprocess}
#SBATCH --cpus-per-task=${nCPUs}
#SBATCH --time=24:00:00
#SBATCH --output=${logdir}/${ana}_${RUNNAME}_${preprocess}.out
#SBATCH --error=${logdir}/${ana}_${RUNNAME}_${preprocess}.err
$slurmshell
EOF

    cat >> $slurmshell << EOF
#!/bin/tcsh
source /group/clas12/packages/setup.csh
module load clas12/pro
cd $pathtoscipio/macros/process
clas12root -b -q process_pipluspi0.C\(\"${infile}\",\"${outfile}\",$energy\)
echo "Done"
EOF

    sbatch $slurmslurm
    
done
