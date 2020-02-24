Files to run pythia simulations and plot multiplicity dependence of J/psi (or other quarkonium, open heavy flavor or other hard probe) producion.

0. step is to install root and pythia8, telling pythia where to find root while installing. Download pythia from http://home.thep.lu.se/Pythia/ and follow the instructions from the README file
1. step is to produce the pythia events and store the information as TTrees in a root file. This is done by the oniaVsMult.cc file. It has to be compiled with the help of the Makefile, so run 
   make oniaVsMult
   while in the folder. Then the code can just be run with
   ./oniaVsMult
   More information are in the file as comments.
1.a) To run many jobs in parallel on the GSI farm use the scripts runPythia.sh and submit.sh 
     E.g. the following command: 
     ./submit.sh outputFolder 1000 100000 settings.cmnd 
      - will run 1000 jobs
      - each will produce 2*100000 events (the script uses openMP for parallelization)
      - it will use the pythia settings defined in settings.cmd
      - the trees will be written to a folder TREES/outputFolder
      (submit.sh creates the folders and tells slurm to launch 10000 jobs running runPythia.sh, which will then execute ./oniaVsMult
2. The plotting vs multiplicity is done with the plotRelOrigin.C macro, using the trees as input. This has to be run with root in compiled mode, so run
    root plotRelOrigin.C+ 
    it uses the files 
      defineAliases.C   - aliases for the trees are stored
      defineEstimator.C - the multiplicity estimators are defined here, e.g. V0 = V0A + V0C
      defineVariaion.C  - defines what exactly you want to plot, e.g. prompt J/psi in bins of pT, nonprompt J/psi for different production mechanisms, etc.
