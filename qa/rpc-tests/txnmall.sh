#!/usr/bin/env bash

# Test block generation and basic wallet sending

if [ $# -lt 1 ]; then
        echo "Usage: $0 path_to_binaries"
        echo "e.g. $0 ../../src"
        exit 1
fi

ICOIND=${1}/icoind
CLI=${1}/icoin-cli

DIR="${BASH_SOURCE%/*}"
SENDANDWAIT="${DIR}/send.sh"
if [[ ! -d "$DIR" ]]; then DIR="$PWD"; fi
. "$DIR/util.sh"

D=$(mktemp -d test.XXXXX)

# Two nodes; one will play the part of merchant, the
# other an evil transaction-mutating miner.

D1=${D}/node1
CreateDataDir $D1 port=11000 rpcport=11001
I1ARGS="-datadir=$D1 -debug"
$ICOIND $I1ARGS &
I1PID=$!

D2=${D}/node2
CreateDataDir $D2 port=11010 rpcport=11011
I2ARGS="-datadir=$D2 -debug"
$ICOIND $I2ARGS &
I2PID=$!

trap "kill -9 $I1PID $I2PID; rm -rf $D" EXIT

# Wait until all four nodes are at the same block number
function WaitBlocks {
    while :
    do
        sleep 1
        BLOCKS1=$( GetBlocks $I1ARGS )
        BLOCKS2=$( GetBlocks $I2ARGS )
        if (( $BLOCKS1 == $BLOCKS2 ))
        then
            break
        fi
    done
}

# Wait until node has $N peers
function WaitPeers {
    while :
    do
        PEERS=$( $CLI $1 getconnectioncount )
        if (( "$PEERS" == $2 ))
        then
            break
        fi
        sleep 1
    done
}

# Start with I2 connected to I1:
$CLI $I2ARGS addnode 127.0.0.1:11000 onetry
WaitPeers "$I1ARGS" 1

# 1 block, 50 XBT each == 50 XBT
$CLI $I1ARGS setgenerate true 1

WaitBlocks
# 100 blocks, 0 mature == 0 XBT
$CLI $I2ARGS setgenerate true 100
WaitBlocks

CheckBalance $I1ARGS 50
CheckBalance $I2ARGS 0

# restart I2 with no connection
$CLI $I2ARGS stop > /dev/null 2>&1
wait $I2PID
$ICOIND $I2ARGS &
I2PID=$!

I2ADDRESS=$( $CLI $I2ARGS getnewaddress )

# Have I1 create two transactions; second will
# spend change from first, since I1 starts with only a single
# 50 icoin output:
TXID1=$( $CLI $I1ARGS sendtoaddress $I2ADDRESS 1.0 )
TXID2=$( $CLI $I1ARGS sendtoaddress $I2ADDRESS 2.0 )

# Mutate TXID1 and add it to I2's memory pool:
RAWTX1=$( $CLI $I1ARGS getrawtransaction $TXID1 )
RAWTX2=$( $CLI $I1ARGS getrawtransaction $TXID2 )
# ... mutate RAWTX1:
# RAWTX1 is hex-encoded, serialized transaction. So each
# byte is two characters; we'll prepend the first
# "push" in the scriptsig with OP_PUSHDATA1 (0x4c),
# and add one to the length of the signature.
# Fields are fixed; from the beginning:
# 4-byte version
# 1-byte varint number-of inputs (one in this case)
# 32-byte previous txid
# 4-byte previous output
# 1-byte varint length-of-scriptsig
# 1-byte PUSH this many bytes onto stack
#  ... etc
# So: to mutate, we want to get byte 41 (hex characters 82-83),
# increment it, and insert 0x4c after it.
L=${RAWTX1:82:2}
NEWLEN=$( printf "%x" $(( 16#$L + 1 )) )
MUTATEDTX1=${RAWTX1:0:82}${NEWLEN}4c${RAWTX1:84}
# ... give mutated tx1 to I2:
MUTATEDTXID=$( $CLI $I2ARGS sendrawtransaction $MUTATEDTX1 )

echo "TXID1: " $TXID1
echo "Mutated: " $MUTATEDTXID

# Re-connect nodes, and have I2 mine a block
$CLI $I2ARGS addnode 127.0.0.1:11000 onetry
WaitPeers "$I1ARGS" 1

$CLI $I2ARGS setgenerate true 1
WaitBlocks

$CLI $I2ARGS stop > /dev/null 2>&1
wait $I2PID
$CLI $I1ARGS stop > /dev/null 2>&1
wait $I1PID

trap "" EXIT

echo "Done, icoind's shut down. To rerun/poke around:"
echo "${1}/icoind -datadir=$D1 -daemon"
echo "${1}/icoind -datadir=$D2 -daemon -connect=127.0.0.1:11000"
echo "To cleanup:"
echo "killall icoind; rm -rf test.*"
exit 0

echo "Tests successful, cleaning up"
rm -rf $D
exit 0
