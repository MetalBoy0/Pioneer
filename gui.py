import tkinter
from tkinter import ttk, messagebox
from threading import Thread
import os
import subprocess
import time
from PIL import Image, ImageTk


root: tkinter.Tk
consoleTextInput: tkinter.Listbox

startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w kqKQ -"  # "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R"
running = True
consoleText = []
consoleCursor = 0

WORKER: Thread
THINKING = False
DEBUG_MODE: bool = False
SHOWING_BB: int | None = None
PLAYING: bool = False
AIPATH = os.getcwd() + "\\.vscode\\Debug.exe"
AI = subprocess.Popen([AIPATH], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
AI.stdout.flush()
AI.stdout.readline()

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
        alpha = int(0.5 * 255)
        fillRed = rgb(255, 0, 0)
        fillRed = root.winfo_rgb(fillRed)
        fillBlue = rgb(0, 0, 200)
        fillBlue = root.winfo_rgb(fillBlue)
        imageRed = Image.new("RGBA", (640 // 8 + 1, 640 // 8 + 1), fillRed)
        imageBlue = Image.new("RGBA", (640 // 8 + 1, 640 // 8 + 1), fillBlue)
        imageRed.putalpha(int(0.5 * 255))
        imageBlue.putalpha(int(0.5 * 255))

        self.imageRed = ImageTk.PhotoImage(imageRed)
        self.imageBlue = ImageTk.PhotoImage(imageBlue)
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
    def __init__(self, start: int, end: int, castle=False, enPassant=False):
        self.start = start
        self.end = end
        self.castle = castle
        self.enPassant = enPassant

    def __str__(self):
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

    def getMoves(self) -> list[Move]:
        global AI, consoleCursor, consoleText
        AI.stdin.write("go perft 1\n".encode())
        AI.stdin.flush()
        AI.stdout.flush()
        if DEBUG_MODE:
            typeToConsole("GUI> go perft 1")
        moves = []
        d = False
        time.sleep(0.5)
        for output_line in iter(AI.stdout):
            print(output_line)
            output_line = output_line.decode().strip()
            AI.stdout.flush()
            if output_line == "":
                continue
            if "Perft search to depth" in output_line:
                d = True
            if "-" in output_line:
                break
            if not d:
                if "e1c1" in output_line and self.board.board[4][0].piece == WC | K:
                    moves.append(Move(4, 2, True))
                elif "e1g1:" in output_line and self.board.board[4][0].piece == WC | K:
                    moves.append(Move(4, 6, True))
                elif "e8c8" in output_line and self.board.board[4][7].piece == BC | K:
                    moves.append(Move(60, 58, True))
                elif "e8g8" in output_line and self.board.board[4][7].piece == BC | K:
                    moves.append(Move(60, 62, True))

                else:
                    moves.append(
                        Move(
                            (ord(output_line[0]) - 97 + (int(output_line[1]) - 1) * 8),
                            (ord(output_line[2]) - 97 + (int(output_line[3]) - 1) * 8),
                        )
                    )

        self.board.moves = moves
        if DEBUG_MODE:
            if moves != []:
                typeToConsole(f"Pioneer> {moves[0]}... Amount: {len(moves)}")
            typeToConsole(consoleText[consoleCursor])
        return moves

    def makeMove(self, move: Move):
        string = "makemove " + str(move) + "\n"
        print(string)
        AI.stdin.write((string).encode())
        AI.stdin.flush()
        AI.stdout.readline()
        AI.stdout.readline()
        if move.end == self.board.enPassant:
            move.enPassant = True
        self.board.enPassant = None

        if (
            getPiece(self.board.board[move.start % 8][move.start // 8].piece) == P
            and abs(move.start - move.end) == 16
        ):
            self.board.enPassant = move.start + (8 if self.board.isWhiteTurn else -8)
        if move.castle:
            if move.end == 6 or move.end == 62:
                sideAdd = 0 if self.board.isWhiteTurn else 56
                self.board.movePiece(7 + sideAdd, 5 + sideAdd)
                self.board.movePiece(4 + sideAdd, 6 + sideAdd)
            else:
                sideAdd = 0 if self.board.isWhiteTurn else 56
                self.board.movePiece(0 + sideAdd, 3 + sideAdd)
                self.board.movePiece(4 + sideAdd, 2 + sideAdd)

        elif move.enPassant:
            self.board.movePiece(move.start, move.end)
            enemyPawn = move.end + (-8 if self.board.isWhiteTurn else 8)
            self.board.board[enemyPawn % 8][enemyPawn // 8].piece = EMPTY
        else:
            self.board.movePiece(move.start, move.end)

        self.getMoves()
        self.board.isWhiteTurn = not self.board.isWhiteTurn


class Square:
    def __init__(self, x: int, y: int, canvas: tkinter.Canvas):
        self.coords = (x, y)
        self.index = (7 - x) + y * 8
        self.piece = EMPTY
        self.isMoveable = False
        self.selected = False
        self.canvas: tkinter.Canvas = canvas
        self.image = None
        self.isLastMoved = False
        self.castle = False

    def update(self, width, height, bitboard):
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
        if bitboard:
            self.canvas.create_image(
                (x + 640 // 8 // 2, 560 - y + 640 // 8 // 2),
                image=(
                    IMAGES.imageRed
                    if (bitboard >> self.index) & 1
                    else IMAGES.imageBlue
                ),
            )


class Board:
    def __init__(self, canvas: tkinter.Canvas):
        self.selectedBB = None
        self.canvas = canvas
        self.board = [[Square(x, y, self.canvas) for y in range(8)] for x in range(8)]
        self.fen = startingFen
        self.isWhiteTurn = True
        self.moves = []
        self.enPassant = None
        self.selPieceMoves = []
        self.selected = (-1, -1)
        self.lastMove = None
        self.loadFen(self.fen)
        self.update()

    def loadFen(self, fen: str):
        self.fen = fen
        i = 0
        fen = fen.split(" ")
        for f in fen[0]:
            if f == "/":
                continue
            if f.isdigit():
                for _ in range(int(f)):
                    self.board[7 - i % 8][7 - i // 8].piece = EMPTY
                    i += 1
                continue

            x = 7 - i % 8
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
            else:
                self.board[7 - x][7 - y].piece = EMPTY
            i += 1
        self.isWhiteTurn = fen[1] == "w"

    def getBB(self, i):
        o = getOutput("debug print bitboard")

        bbs = []
        for x in range(10):
            bbs.append([])
            for _, x in enumerate(o[i * 9 : i * 9 + 9]):
                if not ("1" in x or "0" in x):
                    continue
                bbs[-1].append(x)

        o = getOutput("debug print attackedBB")

        bbs.append([])
        for _, x in enumerate(o):
            if not ("1" in x or "0" in x):
                continue
            bbs[-1].append(x)
        bb = bbs[i]

        bb = "".join(bb)
        bb = bb.replace("\r\n", "")
        bb = bb.replace(" ", "")
        bb = int(bb, base=2)
        self.selectedBB = bb

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
        global WORKER, THINKING
        x = event.x // (640 // 8)
        y = 7 - event.y // (640 // 8)
        print(x + y * 8)
        if THINKING:
            return
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
        if PLAYING and not self.isWhiteTurn:
            WORKER.start()

    def update(self):
        global SHOWING_BB
        if SHOWING_BB != None:
            self.getBB(SHOWING_BB)
        else:
            self.selectedBB = None
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
                self.board[x][y].update(640 // 8, 640 // 8, self.selectedBB)


class ConsoleReturn:
    def __init__(self, error: bool, message: str):
        self.error: bool = error
        self.message: str = message


MANAGER: Manager
IMAGES: Images


def _exit(root, boardCanvas):
    global running
    if messagebox.askyesno("Exit", "Are you sure you want to exit?"):
        AI.kill()
        running = False


def processCommand(string) -> ConsoleReturn:
    global DEBUG_MODE, SHOWING_BB, PLAYING
    string = string[2:].strip().split(" ")
    if string[0] == "showBB":
        if len(string) != 2:
            return ConsoleReturn(True, "no bitboard specified")

        bbi = {
            "pawn": 1,
            "knight": 2,
            "bishop": 3,
            "rook": 4,
            "queen": 5,
            "king": 6,
            "empty": 0,
            "all": 7,
            "black": 9,
            "white": 8,
            "attacked": 10,
            "none": None,
        }

        t = string[1].lower()
        if t[-1] == "s":
            t = t[:-1]

        SHOWING_BB = bbi[t]

        return ConsoleReturn(False, "")
    elif string[0] == "debugmode":
        if len(string) != 2:
            return ConsoleReturn(True, "Invalid parameter")
        if string[1].lower() in ["false", "0"]:
            DEBUG_MODE = False
        elif string[1].lower() in ["true", "1"]:
            DEBUG_MODE = True
        else:
            return ConsoleReturn(True, "Invalid parameter")
        return ConsoleReturn(False, "")
    elif string[0] == "":
        return ConsoleReturn(False, "")
    elif string[0].lower() == "play":
        MANAGER.board.loadFen(startingFen)
        getOutput("position fen " + startingFen)
        MANAGER.getMoves()
        MANAGER.board.selected = (-1, -1)
        MANAGER.board.lastMove = None
        PLAYING = True
        return ConsoleReturn(False, "Activated AI")
    else:
        return ConsoleReturn(True, "Unknown Command")


def worker():
    global MANAGER, THINKING, WORKER
    THINKING = True
    move = getAIOutput(6)
    MANAGER.makeMove(move)
    THINKING = False
    WORKER = Thread(target=worker, daemon=True)


def typeToConsole(string):
    global consoleTextInput
    consoleTextInput.insert(tkinter.END, string)


def getOutput(string: str, timeout=0.1) -> list[str]:
    global AI, DEBUG_MODE
    if DEBUG_MODE:
        typeToConsole(f"GUI> {string}")
    AI.stdin.write((string + "\n").encode())
    AI.stdin.flush()
    time.sleep(timeout)
    l = []

    while True:
        AI.stdout.flush()
        output_line = AI.stdout.readline().decode()

        l.append(output_line)

        if "-" == output_line.strip():
            break
    if DEBUG_MODE:
        if l != ["\r\n"]:
            typeToConsole(f"Pioneer> ")
            for o in l:
                typeToConsole(o)
    return l


def getAIOutput(depth: int):
    out = getOutput("go depth {}".format(depth), 1)
    print("".join(out))
    move = out[-3].split(" ")[2]
    return Move(
        (ord(move[0]) - 97 + (int(move[1]) - 1) * 8),
        (ord(move[2]) - 97 + (int(move[3]) - 1) * 8),
    )


def appendToConsole(string: str):
    global consoleText, consoleTextInput, consoleCursor

    consoleTextInput.delete(tkinter.END)
    consoleText[-1] = consoleText[-1] + string
    consoleTextInput.insert(tkinter.END, consoleText[-1])
    consoleTextInput.yview_moveto(1)


def backspaceConsole(console: tkinter.Listbox):
    global consoleText, consoleTextInput, consoleCursor
    if len(consoleText[-1]) > 2:
        console.delete(tkinter.END)
        consoleText[-1] = consoleText[-1][:-1]
        consoleTextInput.insert(tkinter.END, consoleText[-1])
        consoleTextInput.yview_moveto(1)


def incrementConsole(val: int):
    global consoleText, consoleTextInput, consoleCursor

    if consoleCursor >= 0 and val > 0 and consoleCursor < len(consoleText) - 1:
        consoleCursor += 1
        consoleText[-1] = consoleText[consoleCursor]
        consoleTextInput.delete(tkinter.END)
        consoleTextInput.insert(tkinter.END, consoleText[-1])

    elif consoleCursor <= len(consoleText) - 1 and val < 0 and consoleCursor > 0:
        consoleCursor -= 1
        consoleText[-1] = consoleText[consoleCursor]
        consoleTextInput.delete(tkinter.END)
        consoleTextInput.insert(tkinter.END, consoleText[-1])


def enterConsole(console: tkinter.Listbox):
    global consoleText, consoleTextInput, consoleCursor
    out = processCommand(consoleText[-1])
    if out.message != "":
        if out.error:
            typeToConsole(f"Error: {out.message}")
        else:
            typeToConsole(out.message)
    if consoleText[-1] != "> ":
        consoleText.append("> ")
        consoleCursor += 1
    print(consoleTextInput.size())
    consoleTextInput.insert(tkinter.END, consoleText[-1])
    consoleTextInput.yview_moveto(1)


def getConsoleOut(userIn, console: tkinter.Listbox):
    out = getOutput(userIn)
    out = "".join(out)


def main():
    global AI, MANAGER, IMAGES, WORKER, consoleTextInput, root

    root = tkinter.Tk()
    root.title("Chess")
    root.geometry("940x640")

    boardCanvas = tkinter.Canvas(root, width=640, height=640)
    boardCanvas.config(borderwidth=1, relief=tkinter.SUNKEN)

    consoleFrame = tkinter.Frame(root, width=300, height=300, bg="black")
    consoleFrame.config(borderwidth=1, relief=tkinter.SUNKEN)
    consoleFrame.pack_propagate(False)
    consoleScroll = tkinter.Scrollbar(consoleFrame)
    consoleTextInput = tkinter.Listbox(consoleFrame)
    consoleTextInput.config(
        bg="black",
        fg="white",
        highlightcolor="black",
        highlightthickness=0,
        selectbackground="black",
        activestyle=tkinter.NONE,
    )
    consoleTextInput.config(yscrollcommand=consoleScroll.set)
    consoleScroll.config(command=consoleTextInput.yview)
    consoleScroll.pack(side=tkinter.RIGHT, fill=tkinter.Y)
    consoleTextInput.pack(fill=tkinter.BOTH, expand=True)
    consoleFrame.pack(side=tkinter.RIGHT, anchor=tkinter.S)
    boardCanvas.pack(side=tkinter.LEFT, anchor=tkinter.NW)


    for x in [chr(i) for i in range(33, 127)]:
        if x == ">":
            consoleTextInput.bind("<greater>", lambda e, c=x: appendToConsole(c))
        elif x == "<":
            consoleTextInput.bind("<less>", lambda e, c=x: appendToConsole(c))
        else:
            consoleTextInput.bind(x, lambda e, c=x: appendToConsole(c))
    consoleTextInput.bind("<space>", lambda x: appendToConsole(" "))
    consoleTextInput.bind("<BackSpace>", lambda x: backspaceConsole(consoleTextInput))
    consoleTextInput.bind("<Return>", lambda e: enterConsole(consoleTextInput))
    consoleTextInput.bind("<Up>", lambda e: incrementConsole(-1))
    consoleTextInput.bind("<Down>", lambda e: incrementConsole(1))
    root.protocol("WM_DELETE_WINDOW", lambda: _exit(root, boardCanvas))

    IMAGES = Images()
    MANAGER = Manager(boardCanvas)
    MANAGER.getMoves()
    getOutput("position fen " + startingFen)
    print("".join(getOutput("d")))
    MANAGER.getMoves()

    root.bind("<Button-1>", MANAGER.board.onCLick)

    WORKER = Thread(target=worker, daemon=True)
    consoleText.append("> ")
    consoleTextInput.insert(tkinter.END, consoleText[-1])
    while running:
        boardCanvas.delete("all")
        MANAGER.board.update()

        boardCanvas.update()


main()
