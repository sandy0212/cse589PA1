#!/bin/bash

#Title           :assignment1_update_verifier.sh
#description     :This script will update the verifier scripts for assignment1.
#Author		     :Swetank Kumar Saha <swetankk@buffalo.edu>
#Version         :1.0
#====================================================================================

cd grader
wget -r -c https://github.com/qiqingh/CSE4-589/raw/main/grader_controller -O grader_controller
