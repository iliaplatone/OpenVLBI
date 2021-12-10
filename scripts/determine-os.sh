#! /bin/bash
# return an awkable string consisting of
#    unix OS type, or
#    Linux dist, or
#    a long guess (based on /proc), or
#    no clue

giveUp () {
   echo "Unknown"
   exit 0
}

# keep this easily awkable, prepending an initial clue
versionGuess () {
   if [ -e /proc/version ]; then
      echo -n "Unsure "
      cat /proc/version
      exit 0
   fi
   return 1
}

# if we have ignition, print and exit
gotDist () {
   [ -n "$1" ] && echo "$1" && exit 0
}

# we are only interested in a single word "dist" here
# various malformations can occur; admin will have to code appropately based on output
linuxRelease () {
   if [ -r /etc/lsb-release ]; then
      dist=$(grep 'DISTRIB_ID' /etc/lsb-release | sed 's/DISTRIB_ID=//' | head -1)
      gotDist "$dist"
   fi

   dist=$(find /etc/ -maxdepth 1 -name '*release' 2> /dev/null | sed 's/\/etc\///' | sed 's/-release//' | head -1)
   gotDist "$dist"

   dist=$(find /etc/ -maxdepth 1 -name '*version' 2> /dev/null | sed 's/\/etc\///' | sed 's/-version//' | head -1)
   gotDist "$dist"

   return 1
}

# start with uname and branch the decision from there
dist=$(uname -s 2> /dev/null)
if [ "$dist" = "Linux" ]; then
   linuxRelease
   versionGuess
   giveUp
elif [ -n "$dist" ]; then
   echo "$dist"
   exit 0
else
   versionGuess
   giveUp
fi

# we shouldn't get here
giveUp
# done
