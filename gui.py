import tkinter
from tkinter import ttk, messagebox
import os, sys
from io import StringIO
import subprocess
import time
from PIL import Image, ImageTk


startingFen = "rnbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKQBNR"
running = True


EMPTY = 0
WC = 0
BC = 8
P = 1
N = 2
B = 3
R = 4
Q = 5
K = 6


def rgb(r, g, b):
    return "#%02x%02x%02x" % (r, g, b)


def isWhite(a):
    return a & 8 == 0


def getPiece(a):
    return a & 7


def invertY(i):
    y = i // 8
    x = i % 8
    return x + (7 - y) * 8


class Images:
    def __init__(self):
        pieces = Image.open("ChessPieces.png")
        WKingIM = pieces.crop((0, 0, 2000 // 6, 667 // 2)).resize((640 // 8, 640 // 8))
        WQueenIM = pieces.crop((2000 // 6, 0, 2000 // 6 * 2, 667 // 2)).resize(
            (640 // 8, 640 // 8)
        )
        WBishopIM = pieces.crop((2 * 2000 // 6, 0, 2000 // 6 * 3, 667 // 2)).resize(
            (640 // 8, 640 // 8)
        )
        WKnightIM = pieces.crop((3 * 2000 // 6, 0, 2000 // 6 * 4, 667 // 2)).resize(
            (640 // 8, 640 // 8)
        )
        WRookIM = pieces.crop((4 * 2000 // 6, 0, 2000 // 6 * 5, 667 // 2)).resize(
            (640 // 8, 640 // 8)
        )
        WPawnIM = pieces.crop((5 * 2000 // 6, 0, 2000, 667 // 2)).resize(
            (640 // 8, 640 // 8)
        )
        BKingIM = pieces.crop((0, 667 // 2, 2000 // 6, 667)).resize(
            (640 // 8, 640 // 8)
        )
        BQueenIM = pieces.crop((2000 // 6, 667 // 2, 2000 // 6 * 2, 667)).resize(
            (640 // 8, 640 // 8)
        )
        BBishopIM = pieces.crop((2 * 2000 // 6, 667 // 2, 2000 // 6 * 3, 667)).resize(
            (640 // 8, 640 // 8)
        )
        BKnightIM = pieces.crop((3 * 2000 // 6, 667 // 2, 2000 // 6 * 4, 667)).resize(
            (640 // 8, 640 // 8)
        )
        BRookIM = pieces.crop((4 * 2000 // 6, 667 // 2, 2000 // 6 * 5, 667)).resize(
            (640 // 8, 640 // 8)
        )
        BPawnIM = pieces.crop((5 * 2000 // 6, 667 // 2, 2000, 667)).resize(
            (640 // 8, 640 // 8)
        )

        self.WKingIM = ImageTk.PhotoImage(WKingIM)
        self.WQueenIM = ImageTk.PhotoImage(WQueenIM)
        self.WBishopIM = ImageTk.PhotoImage(WBishopIM)
        self.WRookIM = ImageTk.PhotoImage(WRookIM)
        self.WKnightIM = ImageTk.PhotoImage(WKnightIM)
        self.WPawnIM = ImageTk.PhotoImage(WPawnIM)
        self.BKingIM = ImageTk.PhotoImage(BKingIM)
        self.BQueenIM = ImageTk.PhotoImage(BQueenIM)
        self.BBishopIM = ImageTk.PhotoImage(BBishopIM)
        self.BKnightIM = ImageTk.PhotoImage(BKnightIM)
        self.BRookIM = ImageTk.PhotoImage(BRookIM)
        self.BPawnIM = ImageTk.PhotoImage(BPawnIM)
        self.images = {
            1: self.WPawnIM,
            2: self.WKnightIM,
            3: self.WBishopIM,
            4: self.WRookIM,
            5: self.WQueenIM,
            6: self.WKingIM,
            9: self.BPawnIM,
            10: self.BKnightIM,
            11: self.BBishopIM,
            12: self.BRookIM,
            13: self.BQueenIM,
            14: self.BKingIM,
        }


class Move:
    def __init__(self, start: int, end: int, castle=False):
        self.start = start
        self.end = end
        self.castle = castle

    def __str__(self):
        if self.castle and (self.end - self.start > 0):
            return "O-O"
        elif self.castle and (self.end - self.start < 0):
            return "O-O-O"
        return (
            chr(self.start % 8 + 97)
            + str(self.start // 8 + 1)
            + chr(self.end % 8 + 97)
            + str(self.end // 8 + 1)
        )

    def __repr__(self):
        return str(self)


class Manager:
    def __init__(self, canvas: tkinter.Canvas):
        self.board = Board(canvas)
        self.moves = []

    def getMoves(self) -> list[Move]:
        AI.stdin.write("\n".encode())
        AI.stdin.write("go perft 1\n".encode())
        AI.stdin.flush()
        moves = []
        d = False
        time.sleep(0.1)
        while True:
            output_line = AI.stdout.readline().decode().strip()
            print(output_line)
            AI.stdout.flush()
            if output_line == "":
                AI.stdout.readline()
                continue
            if "-" in output_line and "O-O" not in output_line:
                break
            elif "Total nodes:" in output_line:
                d = True
            if not d:
                if "O-O" in output_line:
                    moves.append(Move(4, 6, True))
                else:
                    moves.append(
                        Move(
                            (ord(output_line[0]) - 97 + (int(output_line[1]) - 1) * 8),
                            (ord(output_line[2]) - 97 + (int(output_line[3]) - 1) * 8),
                        )
                    )

        self.board.moves = moves
        return moves

    def makeMove(self, move: Move):
        self.moves.append(move)
        string = "makemove " + str(move)
        print(string)
        AI.stdin.write((string).encode())
        if move.castle and (move.end - move.start > 0):
            sideAdd = 0 if self.board.isWhiteTurn else 56
            self.board.movePiece(7 + sideAdd, 5 + sideAdd)
            self.board.movePiece(4 + sideAdd, 6 + sideAdd)
        else:
            self.board.movePiece(move.start, move.end)
        self.getMoves()
        self.board.isWhiteTurn = not self.board.isWhiteTurn
        print("".join(getOutput("d")))

    def undoMove(self):
        move = self.moves[-1]
        self.moves.pop()
        if move.castle and (move.end - move.start > 0):
            sideAdd = 0 if self.board.isWhiteTurn else 56
            self.board.movePiece(5 + sideAdd, 7 + sideAdd)
            self.board.movePiece(6 + sideAdd, 4 + sideAdd)
        else:
            self.board.movePiece(move.end, move.start)
        AI.stdin.write("undomove\n".encode())
        self.board.isWhiteTurn = not self.board.isWhiteTurn
        self.getMoves()
        print("undomove")

class Square:
    def __init__(self, x: int, y: int, canvas: tkinter.Canvas):
        self.coords = (x, y)
        self.index = x + y * 8
        self.piece = EMPTY
        self.isMoveable = False
        self.selected = False
        self.canvas: tkinter.Canvas = canvas
        self.image = None
        self.isLastMoved = False
        self.castle = False

    def update(self, width, height):
        x = self.coords[0] * width
        y = self.coords[1] * height
        color = (
            rgb(237, 216, 192)
            if (self.coords[0] + self.coords[1]) % 2 == 0
            else rgb(171, 122, 101)
        )
        color = (
            (
                rgb(217, 199, 112)
                if (self.coords[0] + self.coords[1]) % 2 == 0
                else rgb(193, 147, 73)
            )
            if self.selected or self.isLastMoved
            else color
        )
        color = (
            (
                rgb(212, 58, 72)
                if (self.coords[0] + self.coords[1]) % 2 == 0
                else rgb(176, 39, 48)
            )
            if self.isMoveable
            else color
        )
        self.canvas.create_rectangle(
            x, 560 - y, x + width, 560 - y + height, fill=color, outline=color
        )

        if self.piece != 0:
            self.image = IMAGES.images[self.piece]
            self.canvas.create_image(
                (x + 640 // 8 // 2, 560 - y + 640 // 8 // 2), image=self.image
            )
        else:
            self.image = None
        v = False


class Board:
    def __init__(self, canvas: tkinter.Canvas):
        self.canvas = canvas
        self.board = [[Square(x, y, self.canvas) for y in range(8)] for x in range(8)]
        self.fen = startingFen
        self.isWhiteTurn = True
        self.moves = []
        self.selPieceMoves = []
        self.selected = (-1, -1)
        self.lastMove = None
        self.loadFen(self.fen)
        self.update()

    def loadFen(self, fen: str):
        self.fen = fen
        i = 0
        for f in fen:
            if f == "/":
                continue
            if f.isdigit():
                i += int(f)
                continue

            x = i % 8
            y = i // 8

            if f.lower() == f:
                color = BC
            else:
                color = WC
            if f.lower() == "p":
                self.board[7 - x][7 - y].piece = color | P
            elif f.lower() == "n":
                self.board[7 - x][7 - y].piece = color | N
            elif f.lower() == "b":
                self.board[7 - x][7 - y].piece = color | B
            elif f.lower() == "r":
                self.board[7 - x][7 - y].piece = color | R
            elif f.lower() == "q":
                self.board[7 - x][7 - y].piece = color | Q
            elif f.lower() == "k":
                self.board[7 - x][7 - y].piece = color | K
            i += 1

    def movePiece(self, start: int, end: int):
        if self.lastMove != None:
            self.board[self.lastMove.end % 8][
                self.lastMove.end // 8
            ].isLastMoved = False
            self.board[self.lastMove.start % 8][
                self.lastMove.start // 8
            ].isLastMoved = False

        self.lastMove = Move(start, end)
        self.board[end % 8][end // 8].piece = self.board[start % 8][start // 8].piece
        self.board[start % 8][start // 8].piece = 0
        self.board[end % 8][end // 8].isLastMoved = True
        self.board[start % 8][start // 8].isLastMoved = True

    def onCLick(self, event):
        x = event.x // (640 // 8)
        y = 7 - event.y // (640 // 8)
        print(x + y * 8)
        # Make sure you select a piece of your color
        if (
            isWhite(self.board[x][y].piece) == self.isWhiteTurn
            and getPiece(self.board[x][y].piece) != EMPTY
        ):
            if (x, y) != self.selected:
                self.board[x][y].selected = True
            elif self.selected == (x, y):
                self.board[x][y].selected = False
            if self.selected != (-1, -1):
                self.board[self.selected[0]][self.selected[1]].selected = False
            self.selected = (x, y) if self.board[x][y].selected else (-1, -1)
        elif self.selected != (-1, -1) and self.board[x][y].isMoveable:
            MANAGER.makeMove(
                Move(
                    self.selected[0] + self.selected[1] * 8,
                    x + y * 8,
                    self.board[x][y].castle,
                )
            )
            self.board[self.selected[0]][self.selected[1]].selected = False
            self.selected = (-1, -1)

        self.update()

    def update(self):
        movesAll = [
            move.end
            for move in self.moves
            if move.start == self.selected[0] + self.selected[1] * 8
        ]
        castle = [
            move.end
            for move in self.moves
            if move.start == self.selected[0] + self.selected[1] * 8 and move.castle
        ]
        self.selPieceMoves = movesAll
        for x in range(8):
            for y in range(8):
                if x + y * 8 in movesAll:
                    self.board[x][y].isMoveable = True
                else:
                    self.board[x][y].isMoveable = False
                if x + y * 8 in castle:
                    self.board[x][y].castle = True
                else:
                    self.board[x][y].castle = False
                self.board[x][y].update(640 // 8, 640 // 8)


def getOutput(string: str) -> list[str]:
    global AI
    AI.stdout.flush()
    AI.stdin.write("\n".encode())
    AI.stdin.write((string + "\n").encode())
    AI.stdin.flush()
    time.sleep(0.1)
    l = []
    while True:
        output_line = AI.stdout.readline().decode()
        if "-" in output_line:
            break
        l.append(output_line)
    return l


AIPATH = os.getcwd() + "\\.vscode\\a.exe"

MANAGER: Manager
IMAGES: Images
AI = subprocess.Popen([AIPATH], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
AI.stdout.flush()
AI.stdout.readline()


def _exit(root, boardCanvas):
    global running
    if messagebox.askyesno("Exit", "Are you sure you want to exit?"):
        AI.kill()
        running = False


def getConsoleOut(userIn, console: tkinter.Listbox):
    out = getOutput(userIn)
    out = "".join(out)


def main():
    global MANAGER, IMAGES
    root = tkinter.Tk()
    root.title("Chess")
    root.geometry("940x640")

    boardCanvas = tkinter.Canvas(root, width=640, height=640)
    boardCanvas.config(borderwidth=1, relief=tkinter.SUNKEN)

    consoleFrame = tkinter.Frame(root, width=300, height=300)
    consoleFrame.config(bg="white", borderwidth=1, relief=tkinter.SUNKEN)
    consoleFrame.pack_propagate(False)
    consoleScroll = tkinter.Scrollbar(consoleFrame)
    consoleTextInput = tkinter.Listbox(consoleFrame)
    consoleTextInput.config(
        bd=1,
        bg="black",
        relief=tkinter.SUNKEN,
    )
    consoleTextInput.pack(side=tkinter.BOTTOM, fill=tkinter.X, expand=True)
    consoleScroll.pack(side=tkinter.RIGHT, fill=tkinter.Y)
    consoleFrame.pack(side=tkinter.RIGHT, anchor=tkinter.S)
    boardCanvas.pack(side=tkinter.LEFT, anchor=tkinter.NW)

    IMAGES = Images()
    MANAGER = Manager(boardCanvas)
    MANAGER.getMoves()
    print("".join(getOutput("d")))
    root.bind("<Button-1>", MANAGER.board.onCLick)
    consoleTextInput.bind(
        "<Return>",
    )
    root.bind("<Left>", lambda e: MANAGER.undoMove())
    root.protocol("WM_DELETE_WINDOW", lambda: _exit(root, boardCanvas))

    while running:
        boardCanvas.delete("all")
        MANAGER.board.update()

        boardCanvas.update()


main()
