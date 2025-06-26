#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <limits>
#include <cmath>
#include <chrono>
#include <iomanip>
using namespace std;

const int SIZE = 8;

enum Piece {
    EMPTY,
    WHITE_PAWN, BLACK_PAWN,
    WHITE_KING, BLACK_KING,
    WHITE_ROOK, BLACK_ROOK,
    WHITE_BISHOP, BLACK_BISHOP,
    WHITE_QUEEN, BLACK_QUEEN,
    WHITE_KNIGHT, BLACK_KNIGHT
};

struct Move {
    int fromRow, fromCol, toRow, toCol;
};

struct Board {
    Piece grid[SIZE][SIZE];
};

bool isWhitePiece(Piece p) {
    return p == WHITE_PAWN || p == WHITE_KING || p == WHITE_ROOK || p == WHITE_BISHOP || p == WHITE_QUEEN || p == WHITE_KNIGHT;
}
bool isBlackPiece(Piece p) {
    return p == BLACK_PAWN || p == BLACK_KING || p == BLACK_ROOK || p == BLACK_BISHOP || p == BLACK_QUEEN || p == BLACK_KNIGHT;
}

void initializeBoard(Board &b) {
    for(int i = 0; i < SIZE; ++i) {
        for(int j = 0; j < SIZE; ++j) {
            b.grid[i][j] = EMPTY;
        }
    }
    for(int j = 0; j < SIZE; ++j) {
        b.grid[1][j] = BLACK_PAWN;
        b.grid[6][j] = WHITE_PAWN;
    }
    b.grid[0][0] = b.grid[0][7] = BLACK_ROOK;
    b.grid[7][0] = b.grid[7][7] = WHITE_ROOK;

    b.grid[0][2] = b.grid[0][5] = BLACK_BISHOP;
    b.grid[7][2] = b.grid[7][5] = WHITE_BISHOP;

    b.grid[0][1] = b.grid[0][6] = BLACK_KNIGHT;
    b.grid[7][1] = b.grid[7][6] = WHITE_KNIGHT;

    b.grid[0][3] = BLACK_QUEEN;
    b.grid[7][3] = WHITE_QUEEN;

    b.grid[0][4] = BLACK_KING;
    b.grid[7][4] = WHITE_KING;
}

int evaluate(const Board &b) {
    int score = 0;
    for(int i = 0; i < SIZE; ++i) {
        for(int j = 0; j < SIZE; ++j) {
            switch(b.grid[i][j]) {
                case WHITE_PAWN: score += 10; break;
                case BLACK_PAWN: score -= 10; break;
                case WHITE_ROOK: score += 50; break;
                case BLACK_ROOK: score -= 50; break;
                case WHITE_BISHOP: score += 30; break;
                case BLACK_BISHOP: score -= 30; break;
                case WHITE_QUEEN: score += 90; break;
                case BLACK_QUEEN: score -= 90; break;
                case WHITE_KNIGHT: score += 30; break;
                case BLACK_KNIGHT: score -= 30; break;
                case WHITE_KING: score += 1000; break;
                case BLACK_KING: score -= 1000; break;
            }
        }
    }
    return score;
}

void addMovesInDirection(const Board &b, vector<Move> &moves, int r, int c, int dr, int dc, bool isWhite) {
    int i = r + dr, j = c + dc;
    while(i >= 0 && i < SIZE && j >= 0 && j < SIZE) {
        if(b.grid[i][j] == EMPTY) {
            moves.push_back({r, c, i, j});
        } else {
            if(isWhite && isBlackPiece(b.grid[i][j]))
                moves.push_back({r, c, i, j});
            if(!isWhite && isWhitePiece(b.grid[i][j]))
                moves.push_back({r, c, i, j});
            break;
        }
        i += dr;
        j += dc;
    }
}

vector<Move> getPseudoLegalMoves(const Board &b, bool isWhiteTurn) {
    vector<Move> moves;

    for(int i = 0; i < SIZE; ++i) {
        for(int j = 0; j < SIZE; ++j) {
            Piece p = b.grid[i][j];
            if(isWhiteTurn && !isWhitePiece(p)) continue;
            if(!isWhiteTurn && !isBlackPiece(p)) continue;

            if(p == WHITE_PAWN) {
                if(i > 0 && b.grid[i-1][j] == EMPTY) {
                    moves.push_back({i, j, i-1, j});
                    if(i == 6 && b.grid[i-2][j] == EMPTY) {
                        moves.push_back({i, j, i-2, j});
                    }
                }
                if(j > 0 && isBlackPiece(b.grid[i-1][j-1]))
                    moves.push_back({i, j, i-1, j-1});
                if(j < SIZE-1 && isBlackPiece(b.grid[i-1][j+1]))
                    moves.push_back({i, j, i-1, j+1});
            } else if(p == BLACK_PAWN) {
                if(i < SIZE-1 && b.grid[i+1][j] == EMPTY) {
                    moves.push_back({i, j, i+1, j});
                    if(i == 1 && b.grid[i+2][j] == EMPTY) {
                        moves.push_back({i, j, i+2, j});
                    }
                }
                if(j > 0 && isWhitePiece(b.grid[i+1][j-1]))
                    moves.push_back({i, j, i+1, j-1});
                if(j < SIZE-1 && isWhitePiece(b.grid[i+1][j+1]))
                    moves.push_back({i, j, i+1, j+1});
            } else if(p == WHITE_ROOK || p == BLACK_ROOK) {
                for(auto dir : vector<pair<int,int>>{{1,0},{-1,0},{0,1},{0,-1}})
                    addMovesInDirection(b, moves, i, j, dir.first, dir.second, isWhiteTurn);
            } else if(p == WHITE_BISHOP || p == BLACK_BISHOP) {
                for(auto dir : vector<pair<int,int>>{{1,1},{1,-1},{-1,1},{-1,-1}})
                    addMovesInDirection(b, moves, i, j, dir.first, dir.second, isWhiteTurn);
            } else if(p == WHITE_QUEEN || p == BLACK_QUEEN) {
                for(auto dir : vector<pair<int,int>>{{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}})
                    addMovesInDirection(b, moves, i, j, dir.first, dir.second, isWhiteTurn);
            } else if(p == WHITE_KING || p == BLACK_KING) {
                for(int di = -1; di <= 1; ++di) {
                    for(int dj = -1; dj <= 1; ++dj) {
                        if(di == 0 && dj == 0) continue;
                        int ni = i + di, nj = j + dj;
                        if(ni >= 0 && ni < SIZE && nj >= 0 && nj < SIZE) {
                            if(isWhiteTurn && !isWhitePiece(b.grid[ni][nj]))
                                moves.push_back({i, j, ni, nj});
                            if(!isWhiteTurn && !isBlackPiece(b.grid[ni][nj]))
                                moves.push_back({i, j, ni, nj});
                        }
                    }
                }
            } else if(p == WHITE_KNIGHT || p == BLACK_KNIGHT) {
                for(auto offset : vector<pair<int,int>>{{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}}) {
                    int ni = i + offset.first, nj = j + offset.second;
                    if(ni >= 0 && ni < SIZE && nj >= 0 && nj < SIZE) {
                        if(isWhiteTurn && !isWhitePiece(b.grid[ni][nj]))
                            moves.push_back({i, j, ni, nj});
                        if(!isWhiteTurn && !isBlackPiece(b.grid[ni][nj]))
                            moves.push_back({i, j, ni, nj});
                    }
                }
            }
        }
    }
    return moves;
}

pair<int,int> findKing(const Board &b, bool isWhite) {
    for(int i = 0; i < SIZE; ++i) {
        for(int j = 0; j < SIZE; ++j) {
            if(isWhite && b.grid[i][j] == WHITE_KING) return {i, j};
            if(!isWhite && b.grid[i][j] == BLACK_KING) return {i, j};
        }
    }
    return {-1, -1};
}

bool isMoveInCheck(const Board &b, bool isWhiteTurn) {
    auto kingPos = findKing(b, isWhiteTurn);
    if(kingPos.first == -1) return true;

    auto moves = getPseudoLegalMoves(b, !isWhiteTurn);
    for(const Move &m : moves) {
        if(m.toRow == kingPos.first && m.toCol == kingPos.second) {
            return true;
        }
    }
    return false;
}

vector<Move> getValidMoves(const Board &b, bool isWhiteTurn) {
    vector<Move> moves;
    for(const Move &m : getPseudoLegalMoves(b, isWhiteTurn)) {
        Board newBoard = b;
        newBoard.grid[m.toRow][m.toCol] = newBoard.grid[m.fromRow][m.fromCol];
        newBoard.grid[m.fromRow][m.fromCol] = EMPTY;

        if(!isMoveInCheck(newBoard, isWhiteTurn))
            moves.push_back(m);
    }
    return moves;
}

int minimax(Board b, int depth, bool isWhiteTurn, int alpha, int beta) {
    if(depth == 0) return evaluate(b);

    auto moves = getValidMoves(b, isWhiteTurn);
    if(moves.empty()) return evaluate(b);

    if(isWhiteTurn) {
        int maxEval = numeric_limits<int>::min();
        for(const Move &m : moves) {
            Board newBoard = b;
            newBoard.grid[m.toRow][m.toCol] = newBoard.grid[m.fromRow][m.fromCol];
            newBoard.grid[m.fromRow][m.fromCol] = EMPTY;

            int eval = minimax(newBoard, depth-1, false, alpha, beta);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if(beta <= alpha) {
                break;
            }
        }
        return maxEval;

    } else {
        int minEval = numeric_limits<int>::max();
        for(const Move &m : moves) {
            Board newBoard = b;
            newBoard.grid[m.toRow][m.toCol] = newBoard.grid[m.fromRow][m.fromCol];
            newBoard.grid[m.fromRow][m.fromCol] = EMPTY;

            int eval = minimax(newBoard, depth-1, true, alpha, beta);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if(beta <= alpha) {
                break;
            }
        }
        return minEval;
    }
}

Move getBestMove(const Board &b, bool isWhiteTurn) {
    Move bestMove;
    int bestEval = isWhiteTurn ? numeric_limits<int>::min() : numeric_limits<int>::max();

    auto moves = getValidMoves(b, isWhiteTurn);
    for(const Move &m : moves) {
        Board newBoard = b;
        newBoard.grid[m.toRow][m.toCol] = newBoard.grid[m.fromRow][m.fromCol];
        newBoard.grid[m.fromRow][m.fromCol] = EMPTY;

        int eval = minimax(newBoard, 3, !isWhiteTurn,
                           numeric_limits<int>::min(),
                           numeric_limits<int>::max());

        if((isWhiteTurn && eval > bestEval) || (!isWhiteTurn && eval < bestEval)) {
            bestEval = eval;
            bestMove = m;
        }
    }
    return bestMove;
}

void printBoard(const Board &b) {
    cout << "  A B C D E F G H\n";
    for(int i = 0; i < SIZE; ++i) {
        cout << SIZE - i << " ";
        for(int j = 0; j < SIZE; ++j) {
            char symbol = '.';
            switch(b.grid[i][j]) {
                case WHITE_PAWN: symbol = 'P'; break;
                case BLACK_PAWN: symbol = 'p'; break;
                case WHITE_ROOK: symbol = 'R'; break;
                case BLACK_ROOK: symbol = 'r'; break;
                case WHITE_BISHOP: symbol = 'B'; break;
                case BLACK_BISHOP: symbol = 'b'; break;
                case WHITE_QUEEN: symbol = 'Q'; break;
                case BLACK_QUEEN: symbol = 'q'; break;
                case WHITE_KNIGHT: symbol = 'N'; break;
                case BLACK_KNIGHT: symbol = 'n'; break;
                case WHITE_KING: symbol = 'K'; break;
                case BLACK_KING: symbol = 'k'; break;
                default: symbol = '.';
            }
            cout << symbol << " ";
        }
        cout << SIZE - i << '\n';
    }
    cout << "  A B C D E F G H\n\n";
}

void printScores(const Board &b) {
    int total = evaluate(b);
    cout << "Current Score -> White: " << (total >= 0 ? total : 0) << " | Black: " << (total <= 0 ? -total : 0) << '\n';
}

int main() {
    Board b;
    initializeBoard(b);
    bool isWhiteTurn = true;

    long long whiteElapsedTime = 0;
    long long blackElapsedTime = 0;

    while(true) {
        printBoard(b);
        printScores(b);
        cout << "Time -> White: " << fixed << setprecision(1) << (whiteElapsedTime / 1000.0) << "s | Black: " << fixed << setprecision(1) << (blackElapsedTime / 1000.0) << "s\n";

        auto moves = getValidMoves(b, isWhiteTurn);
        if(moves.empty()) {
            cout << (isWhiteTurn ? "White" : "Black") << " has no moves. Game over.\n";
            int finalScore = evaluate(b);
            if(finalScore > 0) cout << "White wins!\n";
            else if(finalScore < 0) cout << "Black wins!\n";
            else cout << "Draw!\n";
            break;
        }

        auto moveStart = chrono::steady_clock::now();

        if(isWhiteTurn) {
            cout << "Enter move (e.g., E2E4): ";
            string moveStr;
            cin >> moveStr;

            if (moveStr.length() != 4 || !isalpha(moveStr[0]) || !isdigit(moveStr[1]) || !isalpha(moveStr[2]) || !isdigit(moveStr[3])) {
                cout << "Invalid move format!\n";
                continue;
            }

            int fromCol = toupper(moveStr[0]) - 'A';
            int fromRow = SIZE - (moveStr[1] - '0');
            int toCol = toupper(moveStr[2]) - 'A';
            int toRow = SIZE - (moveStr[3] - '0');

            bool validMove = false;

            for(const Move &m : moves) {
                if(m.fromRow == fromRow && m.fromCol == fromCol && m.toRow == toRow && m.toCol == toCol) {
                    b.grid[toRow][toCol] = b.grid[fromRow][fromCol];
                    b.grid[fromRow][fromCol] = EMPTY;
                    validMove = true;
                    break;
                }
            }

            if(!validMove) {
                cout << "Invalid move, try again.\n";
                continue;
            }
        } else {
            Move bestMove = getBestMove(b, isWhiteTurn);
            cout << "AI plays: " << char(bestMove.fromCol + 'A') << (SIZE - bestMove.fromRow)
                 << " -> " << char(bestMove.toCol + 'A') << (SIZE - bestMove.toRow) << '\n';
            b.grid[bestMove.toRow][bestMove.toCol] = b.grid[bestMove.fromRow][bestMove.fromCol];
            b.grid[bestMove.fromRow][bestMove.fromCol] = EMPTY;
        }

        auto moveEnd = chrono::steady_clock::now();
        long long turnDuration = chrono::duration_cast<chrono::milliseconds>(moveEnd - moveStart).count();

        if(isWhiteTurn) {
            whiteElapsedTime += turnDuration;
        } else {
            blackElapsedTime += turnDuration;
        }

        isWhiteTurn = !isWhiteTurn;
    }

    return 0;
}
