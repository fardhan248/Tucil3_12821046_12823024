#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

struct Vehicle {
	char id;
	int x, y; // baris, kolom
	bool horizontal;
	int length;
};

struct Puzzle {
	vector<Vehicle> layout;
};

struct State {
	map<char, int> positions;
};

struct KOut {
	int x, y;
};

struct PVhc {
	int x, y;
	bool horizontal;
	int length;
};

PVhc pVhc;
int w, h;
KOut kOut;
int node = 0;

vector<string> split(const string& str, char delim) {
	vector<string> splitted;
	istringstream ss(str);
	string token;
	while (getline(ss, token, delim)) {
		splitted.push_back(token);
	}
	return splitted;
}

void GetInit(vector<string>& vec, int& h, int& w) {
	bool kFound = false;
	if (vec.size() > h) {
		for (int i = 0; i < vec.size(); ++i) {
			for (int j = 0; j < vec[0].size(); ++j) {
				if (vec[i][j] == 'K') {
					vec.erase(vec.begin() + i);  // hapus baris yang mengandung K
					kFound = true;
					break;
				}
			}
			if (kFound) break;
		}
	} else {
		for (int i = 0; i < vec.size(); ++i) {
			if (vec[i].size() > w) {
				bool k = false;
				bool ws = false;
				char toRemove;
				for (int j = 0; j < vec[0].size(); ++j) {
					if (vec[i][j] == ' ') {
						ws = true;
						toRemove = ' ';
					} else if (vec[i][j] == 'K') {
						k = true;
						toRemove = 'K';
					}
				}
				if (ws || k) {
					vec[i].erase(remove(vec[i].begin(), vec[i].end(), toRemove), vec[i].end());
				}
			}
		}
	}
}

void ReadFile(ifstream& file, int& w, int& h, int& nPieces, vector<string>& initial) {
	string line;
	int i = 0;

	while (getline(file, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		vector<string> readLine = split(line, ' ');
		
		if (i == 0 && readLine.size() >= 2) {  // ukuran board
			h = stoi(readLine[0]); // height
			w = stoi(readLine[1]); // width
		} else if (i == 1 && readLine.size() >= 1) { 
			nPieces = stoi(readLine[0]);  // banyak kendaraan
		} else {
			// pakai line biar per huruf
			for (int j = 0; j < line.size(); ++j) {
				if (line[j] == 'K') {
					kOut.x = i - 2;
					kOut.y = j;
				}
			}
			initial.push_back(line);
		}
		++i;
	}
	GetInit(initial, h, w);	
	file.close();
}

void GetInitLayout(vector<string>& initial, int& h, int& w, Puzzle& puzzle) {
	map<char, bool> vIn;
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			if (initial[i][j] == '.') {
				continue;
			}

			if (!vIn[initial[i][j]]) {
				bool isHorizontal = false;

				// cek horizontal/vertikal
				if ((j < w - 1) && (initial[i][j+1] == initial[i][j])) {
					isHorizontal = true;
				} else if ((i < h - 1) && (initial[i+1][j] == initial[i][j])) {
					isHorizontal = false;
				}

				puzzle.layout.emplace_back(Vehicle{initial[i][j], i, j, isHorizontal, 1});
				vIn[initial[i][j]] = true;
				if (initial[i][j] == 'P') {
					pVhc = PVhc{i, j, isHorizontal, 1};
				}
			} else {
				for (auto& layout: puzzle.layout) {
					if (layout.id == initial[i][j]) {
						if ((layout.horizontal && layout.x == i) || (!layout.horizontal && layout.y == j)) {
							layout.length = layout.length + 1;
							if (initial[i][j] == 'P') {
								pVhc.length = pVhc.length + 1;
							}
						}
					}
				}
			}
		}
	}
}

vector<vector<char>> BuildBoard(const Puzzle& puzzle, const State& state, int h, int w) {
	vector<vector<char>> board(h, vector<char>(w, '.'));

	for (const auto& vhc: puzzle.layout) {
		int pos = state.positions.at(vhc.id);  
		int x = vhc.horizontal ? vhc.x : pos;
		int y = vhc.horizontal ? pos : vhc.y;
		
		for (int i = 0; i < vhc.length; ++i) {
			if (vhc.horizontal) {
				board[x][y+i] = vhc.id;
			} else {
				board[x+i][y] = vhc.id;
			}
		}
	}
	return board;
}

struct Successor {
	vector<vector<char>> board;
	char id;
	string dir;
	int cost;
};

// class get successors
class BoardManipulator {
public:
	vector<vector<char>> board;
	vector<Vehicle> vehicles;
	int height, width;

	BoardManipulator(const vector<vector<char>>& initialBoard, const vector<Vehicle>& initialVehicles)
		: board(initialBoard), vehicles(initialVehicles) {
			height = board.size();
			width = board[0].size();
	}

	void UpdateVehiclesFromBoard() {
		for (auto& v : vehicles) {
			bool found = false;
			for (int i = 0; i < height && !found; ++i) {
				for (int j = 0; j < width && !found; ++j) {
					if (board[i][j] == v.id) {
						v.x = i;
						v.y = j;
						found = true;
					}
				}
			}
		}
	}

	vector<vector<char>> MoveRight(const Vehicle& vhc) {
		auto newBoard = board;
		int x = vhc.x;
		int y = vhc.y;
		int rightEdge = y + vhc.length;

		while (rightEdge < width && newBoard[x][rightEdge] == '.') {
			for (int i = 0; i < vhc.length; ++i) {
				newBoard[x][y + i] = '.';
			}
			y++;
			for (int i = 0; i < vhc.length; ++i) {
				newBoard[x][y + i] = vhc.id;
			}
			rightEdge++;
		}

		if (vhc.id == 'P' && vhc.horizontal && kOut.y >= width && x == kOut.x) {
			bool clear = true;
			for (int cy = y + vhc.length; cy < width; ++cy) {
				if (newBoard[x][cy] != '.') {
					clear = false;
					break;
				}
			}
			if (clear) {
				for (int i = 0; i < vhc.length; ++i) {
					if (y + i < width) {
						newBoard[x][y + i] = '.';
					}
				}
			}
		}
		return newBoard;
	}

	vector<vector<char>> MoveLeft(const Vehicle& vhc) {
		auto newBoard = board;
		int x = vhc.x;
		int y = vhc.y;

		while (y > 0 && newBoard[x][y - 1] == '.') {
			for (int i = 0; i < vhc.length; ++i) {
				newBoard[x][y + i] = '.';
			}
			y--;
			for (int i = 0; i < vhc.length; ++i) {
				newBoard[x][y + i] = vhc.id;
			}
		}

		if (vhc.id == 'P' && vhc.horizontal && kOut.y <= 0 && x == kOut.x) {
			bool clear = true;
			for (int cy = y - 1; cy >= 0; --cy) {
				if (newBoard[x][cy] != '.') {
					clear = false;
					break;
				}
			}
			if (clear) {
				for (int i = 0; i < vhc.length; ++i) {
					if (y + i < width) {
						newBoard[x][y + i] = '.';
					}
				}
			}
		}

		return newBoard;
	}

	vector<vector<char>> MoveDown(const Vehicle& vhc) {
		auto newBoard = board;
		int x = vhc.x;
		int y = vhc.y;
		int bottomEdge = x + vhc.length;

		while (bottomEdge < height && newBoard[bottomEdge][y] == '.') {
			for (int i = 0; i < vhc.length; ++i) {
				newBoard[x + i][y] = '.';
			}
			x++;
			for (int i = 0; i < vhc.length; ++i) {
				newBoard[x + i][y] = vhc.id;
			}
			bottomEdge++;
		}

		if (vhc.id == 'P' && !vhc.horizontal && kOut.x >= height && y == kOut.y) {
			bool clear = true;
			for (int cx = x + vhc.length; cx < height; ++cx) {
				if (newBoard[cx][y] != '.') {
					clear = false;
					break;
				}
			}
			if (clear) {
				for (int i = 0; i < vhc.length; ++i) {
					if (x + i < height) {
						newBoard[x + i][y] = '.';
					}
				}
			}
		}
		return newBoard;
	}

	vector<vector<char>> MoveUp(const Vehicle& vhc) {
		auto newBoard = board;
		int x = vhc.x;
		int y = vhc.y;

		while (x > 0 && newBoard[x - 1][y] == '.') {
			for (int i = 0; i < vhc.length; ++i) {
				newBoard[x + i][y] = '.';
			}
			x--;
			for (int i = 0; i < vhc.length; ++i) {
				newBoard[x + i][y] = vhc.id;
			}
		}

		if (vhc.id == 'P' && !vhc.horizontal && kOut.x <= 0 && y == kOut.y) {
			bool clear = true;
			for (int cx = x - 1; cx >= 0; --cx) {
				if (newBoard[cx][y] != '.') {
					clear = false;
					break;
				}
			}
			if (clear) {
				for (int i = 0; i < vhc.length; ++i) {
					if (x + i < height) {
						newBoard[x + i][y] = '.';
					} 
				}
			}
		}
		return newBoard;
	}

	vector<Successor> GetSuccessors() {
		vector<Successor> successors;

		for (const auto& vhc: vehicles) {
			if (vhc.horizontal) {
				auto rightBoard = MoveRight(vhc);
				if (rightBoard != board) {
					successors.push_back({rightBoard, vhc.id, "kanan"});
				}

				auto leftBoard = MoveLeft(vhc);
				if (leftBoard != board) {
					successors.push_back({leftBoard, vhc.id, "kiri"});
				}
			} else {
				auto downBoard = MoveDown(vhc);
				if (downBoard != board) {
					successors.push_back({downBoard, vhc.id, "bawah"});
				}

				auto upBoard = MoveUp(vhc);
				if (upBoard != board) {
					successors.push_back({upBoard, vhc.id, "atas"});
				}
			}
		}
		return successors;
	}
};

void PrintBoard(Successor& s, ostream& out, bool useColor = true) {
	vector<vector<char>> boardState = s.board;

	auto colorWrap = [&](const string& code, char c) -> string {
		if (!useColor) return string(1, c);
		return code + c + "\033[0m";
	};

	auto printChar = [&](int j, int k) {
		char current = boardState[j][k];
		if (current == 'P') {
			out << colorWrap("\033[31m", 'P');
		} else if (current == s.id) {
			out << colorWrap("\033[34m", current);
		} else {
			out << current;
		}
	};

	if (pVhc.horizontal) {
		if (kOut.y == 0) {  // exit di kiri
			for (int j = 0; j < boardState.size(); ++j) {
				out << (j == kOut.x ? colorWrap("\033[32m", 'K') : " ");
				for (int k = 0; k < boardState[0].size(); ++k) {
					printChar(j, k);
				}
				out << endl;
			}
		} else {  // exit di kanan
			for (int j = 0; j < boardState.size(); ++j) {
				for (int k = 0; k < boardState[0].size(); ++k) {
					printChar(j, k);
				}
				out << (j == kOut.x ? colorWrap("\033[32m", 'K') : "");
				out << endl;
			}
		}
	} else {
		if (kOut.x == 0) { // exit di atas
			for (int j = 0; j < boardState.size()+1; ++j) {
				for (int k = 0; k < boardState[0].size(); ++k) {
					if (j == 0) {
						out << (j == kOut.x ? colorWrap("\033[32m", 'K') : " ");
					} else {
						printChar(j - 1, k);
					}
				}
				out << endl;
			}
		} else {  // exit di bawah
			for (int j = 0; j < boardState.size()+1; ++j) {
				for (int k = 0; k < boardState[0].size(); ++k) {
					if (j == h) {
						out << (j == kOut.x ? colorWrap("\033[32m", 'K') : " ");
					} else {
						printChar(j, k);
					}
				}
				out << endl;
			}
		}
	}
}

class RushHourSolver {
	private:
		vector<vector<char>> initialBoard;
		vector<Vehicle> initialVehicles;

	public:
		RushHourSolver(const vector<vector<char>>& board, const vector<Vehicle>& vehicles)
			: initialBoard(board), initialVehicles(vehicles) {}

		string boardToString(const vector<vector<char>>& board) {
			string s;
			for (const auto& row: board) {
				for (char ch: row) s += ch;
			}
			return s;
		}
		
		// Cek goal
		bool IsGoal(const vector<vector<char>>& board) {
			bool PFound = false;
			int px = -1, py = -1;

			for (int i = 0; i < board.size(); ++i) {
				for (int j = 0; j < board[0].size(); ++j) {
					if (board[i][j] == 'P') {
						PFound = true;
						px = i;  // 2
						py = j;  // 1
						break;
					}
				}
				if (PFound) break;
			}

			if (!PFound) return true;
			else return false;
		}

		// g(n): banyak gerakan
		// h(n): banyak blok yang menghalangi P untuk keluar
		int heuristic(const vector<vector<char>>& board) {
			int px = -1, py = -1;
			
			for (int i = 0; i < h; ++i) {
				for (int j = 0; j < w; ++j) {
					if (board[i][j] == 'P') {
						px = i;
						py = j;
						break;
					}
				}
				if (px != -1) break;
			}

			int dx = kOut.x - px; 
			int dy = kOut.y - py; 
			int dirx = (dx != 0) ? dx / abs(dx) : 0;
			int diry = (dy != 0) ? dy / abs(dy) : 0;

			int cx = px + dirx, cy = py + diry;
			int count = 0;

			while (cx >= 0 && cx < board.size() && cy >= 0 && cy < board[0].size()) {
				if (board[cx][cy] != '.' && board[cx][cy] != 'P') count++;
				cx += dirx;
				cy += diry;
			}

			return count;
		}

		// buat path untuk node dari root ke goal
		vector<Successor> BuildPath(const string& goalKey, 
			unordered_map<string, string>& parent, unordered_map<string, 
			Successor>& boardStorage) {

			vector<Successor> path;
			string k = goalKey;
			while (boardStorage.count(k)) {
				path.push_back(boardStorage[k]);
				if (!parent.count(k)) break;
				k = parent[k];
			}
			reverse(path.begin(), path.end());
			return path;
		}

		struct Node {
			Successor board;
			int cost;

			bool operator>(const Node& other) const {
				if (cost != other.cost) {
					return cost > other.cost;
				}
				// jika cost sama, dahulukan state yang menggerakkan P
				if (board.id == 'P' && other.board.id != 'P') return false;
				if (board.id != 'P' && other.board.id == 'P') return true;

				return false;
			}
		};

		vector<Successor> UCS() {
			priority_queue<Node, vector<Node>, greater<Node>> pq;
			unordered_set<string> visited;
			unordered_map<string, string> parent;
			unordered_map<string, Successor> boardStorage;

			Successor initBoard = Successor{initialBoard, '-', ""};
			string startKey = boardToString(initialBoard);

			// tambahkan initial ke queue
			pq.push({initBoard, 0});
			boardStorage[startKey] = initBoard;

			// pencarian loop
			while (!pq.empty()) {
				Node current = pq.top(); pq.pop();  // ambil node prioritas, lalu hapus
				string key = boardToString(current.board.board);
				if (visited.count(key)) continue; // lewati apabila sudah pernah dikunjungi
				visited.insert(key);

				if (IsGoal(current.board.board)) {
					boardStorage[key] = current.board;
					return BuildPath(key, parent, boardStorage);
				}

				BoardManipulator bm(current.board.board, initialVehicles);
				bm.UpdateVehiclesFromBoard();
				auto successors = bm.GetSuccessors();  // bangkitkan node tetangga

				for (auto& succ: successors) {
					string sKey = boardToString(succ.board);
					if (!visited.count(sKey)) {
						int cost = current.cost + 1; // hitung cost
						succ.cost = cost;
						pq.push({succ, cost});
						parent[sKey] = key;
						boardStorage[sKey] = succ;
					}
				}
				++node;
			}
			return {};
		}

		vector<Successor> GBFS() { 
			priority_queue<Node, vector<Node>, greater<Node>> pq;
			unordered_set<string> visited;
			unordered_map<string, string> parent;
			unordered_map<string, Successor> boardStorage; 

			Successor initBoard = Successor{initialBoard, '-', ""};
			string startKey = boardToString(initialBoard);

			pq.push({initBoard, heuristic(initialBoard)});
			boardStorage[startKey] = initBoard;

			while (!pq.empty()) {
				Node current = pq.top(); pq.pop();
				string key = boardToString(current.board.board);
				if (visited.count(key)) continue;
				visited.insert(key);

				if (IsGoal(current.board.board)) {
					boardStorage[key] = current.board;
					return BuildPath(key, parent, boardStorage);
				}

				BoardManipulator bm(current.board.board, initialVehicles);
				bm.UpdateVehiclesFromBoard();
				auto successors = bm.GetSuccessors();

				for (auto& succ: successors) {
					string sKey = boardToString(succ.board);
					if (!visited.count(sKey)) {
						int cost = heuristic(succ.board);
						succ.cost = cost;
						pq.push({succ, cost});
						parent[sKey] = key;
						boardStorage[sKey] = succ;
					}
				}
				++node;
			}
			return {};
		}

		vector<Successor> AStar() { 
			priority_queue<Node, vector<Node>, greater<Node>> pq;
			unordered_map<string, int> gScore;
			unordered_map<string, string> parent;
			unordered_map<string, Successor> boardStorage;

			Successor initBoard = Successor{initialBoard, '-', ""};
			string startKey = boardToString(initialBoard);

			pq.push({initBoard, 0 + heuristic(initBoard.board)});
			gScore[startKey] = 0;
			boardStorage[startKey] = initBoard;

			while (!pq.empty()) {
				Node current = pq.top(); pq.pop();
				string key = boardToString(current.board.board);

				if (IsGoal(current.board.board)) {
					boardStorage[key] = current.board;
					return BuildPath(key, parent, boardStorage);
				}

				BoardManipulator bm(current.board.board, initialVehicles);
				bm.UpdateVehiclesFromBoard();
				auto successors = bm.GetSuccessors();

				for (auto& succ: successors) {
					string sKey = boardToString(succ.board);
					int dummyg = current.cost + 1;

					if (!gScore.count(sKey) || dummyg < gScore[sKey]) {
						int f = dummyg + heuristic(succ.board);
						succ.cost = f;
						gScore[sKey] = dummyg;
						parent[sKey] = key;
						boardStorage[sKey] = succ;
						pq.push({succ, f});
					}
				}
				++node;
			}
			return {};
		}
};

// fungsi untuk memeriksa, apakah kendaraan utama, P, terhalang oleh kendaraan yang sejajar juga menuju K 
bool IsBlocked(vector<vector<char>>& board, Puzzle& puzzle) {
	int dx = kOut.x - pVhc.x;
	int dy = kOut.y - pVhc.y;
	int dirx = (dx != 0) ? dx / abs(dx) : 0;
	int diry = (dy != 0) ? dy / abs(dy) : 0;

	int cx = pVhc.x + dirx;
	int cy = pVhc.y + diry;

	while (cx >= 0 && cx < h && cy >= 0 && cy < w) {
		char c = board[cx][cy];
		if (c != '.' && c != 'P') {
			Vehicle vhc;

			for (const auto& p: puzzle.layout) {
				if (p.id == c) {
					vhc = p;
					break;
				}
			}

			bool isSameOrientation = false;
			if (pVhc.horizontal) {
				if (vhc.horizontal) {
					isSameOrientation = true;
				}
			} else {
				if (!vhc.horizontal) {
					isSameOrientation = true;
				}
			}

			if (isSameOrientation) {
				return true;
			}
		}
		cx += dirx;
		cy += diry;
	}

	return false;
}

int main(int argc, char* argval[]) {
	if (argc < 3) {
		cout << "Masukkan perintah: " << argval[0] << " <nama_file> <algoritma>\n";
		return 1;
	}
	
	string fname = argval[1];  // nama file
	string algo = argval[2];  // algoritma
	ifstream file(fname);

	if (!file.is_open()) {
		cerr << "Eror saat membuka file: " << fname << "\n";
		return 1;
	}
	
	int nPieces;
	Puzzle puzzle;
	vector<string> initial;

	ReadFile(file, w, h, nPieces, initial);
	GetInitLayout(initial, h, w, puzzle);

	State state;
	// getstate
	for (auto& vhc: puzzle.layout) {
		if (vhc.horizontal) {
			state.positions[vhc.id] = vhc.y;
		} else {
			state.positions[vhc.id] = vhc.x;
		}
	}

	ofstream out("result_" + fname);

	if (out.is_open()) {
		auto board = BuildBoard(puzzle, state, h, w);
		if (IsBlocked(board, puzzle)) {
			cout << "Tidak ditemukan solusi.\n";
			out << "Tidak ditemukan solusi.\n";
			return 1;
		}

		RushHourSolver solver(board, puzzle.layout);

		vector<Successor> solution;
		auto start = high_resolution_clock::now();
		if (algo == "ucs") {
			solution = solver.UCS();
		} else if (algo == "gbfs") {
			solution = solver.GBFS();
		} else if (algo == "astar") {
			solution = solver.AStar();
		} else {
			cout << "Masukkan pilihan algoritma: ucs, gbfs, astar\n";
			return 1;
		}
		auto end = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(end - start);

		if (!solution.empty()) {
			cout << "Solusi ditemukan:\n";
				for (int step = 0; step < solution.size(); ++step) {
					auto& s = solution[step];
					if (step == 0) {
						cout << "Papan Awal\n"; 
						out << "Papan Awal\n"; 
					} else {
						cout << "Gerakan " << step << ": " << s.id << "-" << s.dir << endl;
						out << "Gerakan " << step << ": " << s.id << "-" << s.dir << endl;
					}
					PrintBoard(s, cout, true);
					PrintBoard(s, out, false);
					cout << endl;
					out << endl;
				}
				cout << "Jumlah node: " << node << endl;
				cout << "Waktu eksekusi: " << duration.count() << " ms" << endl;

				out << "Jumlah node: " << node << endl;
				out << "Waktu eksekusi: " << duration.count() << " ms" << endl;
		} else {
			cout << "Tidak ditemukan solusi.\n";
			out << "Tidak ditemukan solusi.\n";
		}
	}

	out.close();
	return 0;
}	