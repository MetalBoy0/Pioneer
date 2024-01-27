# The purpose of this script is to compare an engine's perft results with stockfish
# in order to verify the correctness of the engine's move generation.

import subprocess, threading
import time
import os


def getOutput(string: str, AI, write=[]) -> str:
    AI.stdout.flush()
    AI.stdin.write("\n".encode())
    AI.stdin.write((string + "\n").encode())
    AI.stdin.flush()
    time.sleep(1)
    subout = ""
    with AI.stdout:
        for line in iter(AI.stdout.readline, b""):
            if "nodes" in line.decode().lower():
                break
            subout += line.decode()
    return subout


def main():
    timeout = 5
    aiPath = os.getcwd() + "\\.vscode\\a.exe"
    stockfishPath = "C:\\Users\\WVGarrison\\Downloads\\stockfish-windows-x86-64-modern\\stockfish\\stockfish-windows-x86-64-modern.exe"

    positionFen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8" #a2a3 a7a5 a1a2 f2d3
    depth = 5

    stockmoves = ""
    recentMove = ""
    while True:
        ai = subprocess.Popen(
            [aiPath],
            stdout=subprocess.PIPE,
            stdin=subprocess.PIPE,
            close_fds=True,
            shell=True,
        )
        stockfish = subprocess.Popen(
            [stockfishPath], stdout=subprocess.PIPE, stdin=subprocess.PIPE
        )
        ai.stdout.flush()
        ai.stdout.readline()

        stockfish.stdout.flush()
        stockfish.stdout.readline()
        stockstring = f"position fen {positionFen} moves{stockmoves}"
        stockfish.stdin.write("\n".encode())
        stockfish.stdin.write(
            (stockstring + "\n").encode()
        )
        stockfish.stdin.flush()

        ai.stdin.write("\n".encode())
        ai.stdin.write((f"position fen {positionFen}" + "\n").encode())
        ai.stdin.flush()
        ai.stdout.readline()
        ai.stdout.readline()
        ai.stdout.flush()
        
        for x in stockmoves.split(" "):
            if x != "":
                ai.stdin.write((f"makemove {x}" + "\n").encode())
                ai.stdin.flush()
                ai.stdout.flush()

        aiO = getOutput(f"go perft {depth}", ai).strip()
        stockO = getOutput(f"go perft {depth}", stockfish).strip()

        aiO = aiO.replace("\r", "").replace("-",'').strip().split("\n")
        stockfishO = stockO.replace("\r", "").split("\n")

        aiO.sort()
        stockfishO.sort()

        bad = False

        if len(aiO) != len(stockfishO):
            for x in range(min(len(aiO), len(stockfishO))):
                if aiO[x] != stockfishO[x]:
                    print(f"Discrepancy: Stockfish: {stockfishO[x]}, AI: {aiO[x]}")
                    bad = True
                    break
            if not bad:
                print(f"Discrepancy: Stockfish: {stockfishO[x]}, AI: {aiO[x]}")
            print(
                f"Path detected: {stockmoves}\n"
            )
            return
        for i, x in enumerate(aiO):
            num = x.split(" ")[1]
            move = x.split(" ")[0]
            snum = stockfishO[i].split(" ")[1]
            smove = stockfishO[i].split(" ")[0]
            if snum != num:
                print(f"Discrepancy: Stockfish: {stockfishO[i]}, AI: {x}")
                bad = True
                recentMove = smove.replace(":", "")
                break
        if not bad:
            print("All good")
            return
        else:
            stockmoves += f" {recentMove}"
            depth -= 1


if __name__ == "__main__":
    main()
