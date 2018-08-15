#!/bin/sh
# Create example directory structure.
mkdir Annual\ Report
mkdir Annual\ Report/Reports
mkdir Annual\ Report/Reports/2017
touch Annual\ Report/Reports/2017/Annual\ Statement.pdf
touch Annual\ Report/Reports/2017/Budget.pdf
touch Annual\ Report/Reports/2017/Marketing.pdf
mkdir Annual\ Report/Reports/2018
touch Annual\ Report/Reports/2018/Annual\ Statement.pdf
touch Annual\ Report/Reports/2018/Budget.pdf
touch Annual\ Report/Reports/2018/Marketing.pdf
touch Annual\ Report/cat.jpg
# Print user info.
find Annual\ Report
echo "Done!"

