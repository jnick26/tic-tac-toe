#include <fmt/format.h>
#include <array>
#include <functional>

constexpr auto BoardSize = 9;

struct Game {
    enum class Cell {
        _, O, X
    };

    typedef std::array<Cell, BoardSize * BoardSize> Board;

    Board board = {Cell::_};

    Cell at(int i, int j) const {
        return board[i * BoardSize + j];
    }

    Cell &at(int i, int j) {
        return board[i * BoardSize + j];
    }

    Game newGameWith(int i, int j, Cell cell) const {
        Game game = *this;
        game.at(i, j) = cell;
        return game;
    }

    int diagonalRank(Cell cell) const {
        int diagonalRank = 0;
        for (int i = 0; i < BoardSize; ++i) {
            if (at(i, i) == Cell::_) {
                continue;
            }

            if (at(i, i) != cell) {
                return 0;
            }

            diagonalRank++;
        }
        return diagonalRank;
    }

    int backDiagonalRank(Cell cell) const {
        int diagonalRank = 0;
        for (int i = 0; i < BoardSize; ++i) {
            if (at(i, BoardSize - i - 1) == Cell::_) {
                continue;
            }

            if (at(i, BoardSize - i - 1) != cell) {
                return 0;
            }

            diagonalRank++;
        }
        return diagonalRank;
    }

    int rowRank(Cell cell, int row) const {
        int rowRank = 0;
        for (int j = 0; j < BoardSize; ++j) {
            if (at(row, j) == Cell::_) {
                continue;
            }

            if (at(row, j) != cell) {
                return 0;
            }

            rowRank++;
        }
        return rowRank;
    }

    int columnRank(Cell cell, int column) const {
        int columnRank = 0;
        for (int i = 0; i < BoardSize; ++i) {
            if (at(i, column) == Cell::_) {
                continue;
            }

            if (at(i, column) != cell) {
                return 0;
            }

            columnRank++;
        }
        return columnRank;
    }

    int rate(Cell cell) const {
        int boardRank = 0;

        iterateOverLines(cell, [&](int rank) {
            boardRank += rank == BoardSize ? BoardSize * BoardSize : rank;
            return true;
        });

        return boardRank;
    }

    bool winner(Cell cell) {
        bool winner = false;
        iterateOverLines(cell, [&](int rank) {
            winner = rank == BoardSize;
            return !winner;
        });
        return winner;
    }

    void iterateOverLines(Cell cell, std::function<bool(int)> cb) const {
        if (!cb(diagonalRank(cell))) { return; }

        if (!cb(backDiagonalRank(cell))) { return; }

        for (int i = 0; i < BoardSize; ++i) {
            if (!cb(rowRank(cell, i))) { return; }
        }

        for (int j = 0; j < BoardSize; ++j) {
            if (!cb(columnRank(cell, j))) { return; }
        }
    }
};

const char *cellToStr(Game::Cell cell) {
    using _ = Game::Cell;

    switch (cell) {
        case _::O:
            return "O";

        case _::_:
            return "_";

        case _::X:
            return "X";
    }
}

void print(const Game &game) {
    using _ = Game::Cell;

    for (int i = 0; i < BoardSize; ++i) {
        for (int j = 0; j < BoardSize; ++j) {
            fmt::print("{}", cellToStr(game.at(BoardSize - i - 1, j)));
        }
        fmt::print("\n");
    }
}

struct SimpleTurn {
    int i;
    int j;
    int rate;
};

struct Turn {
    int i;
    int j;
};

SimpleTurn find_best_simple(const Game &game, Game::Cell cell) {
    using _ = Game::Cell;

    int best_rate = -1;
    int best_i, best_j;

    for (int i = 0; i < BoardSize; ++i) {
        for (int j = 0; j < BoardSize; ++j) {
            if (game.at(i, j) != _::_) { continue; }

            Game local_game = game;
            local_game.at(i, j) = cell;
            auto local_rate = local_game.rate(cell);
            if (local_rate > best_rate) {
                best_rate = local_game.rate(cell);
                best_i = i;
                best_j = j;
            }
        }
    }

    return {best_i, best_j, best_rate};
}

Turn find_best(const Game &game, Game::Cell cell) {
    using _ = Game::Cell;

    auto[my_i, my_j, my_rate] = find_best_simple(game, cell);
    auto[foe_i, foe_j, for_rate] = find_best_simple(game, cell == _::X ? _::O : _::X);

    fmt::print("Turn '{}', self[{},{}]={}, foe=[{},{}]={}\n", cellToStr(cell), my_j, my_i, my_rate, foe_j, foe_i,
               for_rate);

    if (for_rate > my_rate) {
        return {foe_i, foe_j};
    } else {
        return {my_i, my_j};
    }
}

int main() {
    using _ = Game::Cell;
    Game game;
    auto turn = _::O;
    int iteration_limit = 50;

    while (!game.winner(_::O) && !game.winner(_::X)) {

        auto[i, j] = find_best(game, turn);
        game.at(i, j) = turn;
        print(game);
        fmt::print("********\n");

        if (iteration_limit-- == 0) {
            break;
        }

        turn = turn == _::X ? _::O : _::X;
    }
    return 0;
}
