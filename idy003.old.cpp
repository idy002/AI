#include <bits/stdc++.h>
using namespace std;

const int H = 17;
const int W = 5;
const int Kindcnt = 12;
const int oo = 0x3f3f3f3f;

struct Board {
	int w[H][W];
	inline const int *operator[]( int i ) const { return w[i]; }
	inline int *operator[]( int i ) { return w[i]; }
	void print() const {
		for( int i = 16; i >= 0; i-- ) {
			for( int j = 0; j <= 4; j++ )
				fprintf( stderr, "%d ", w[i][j] );
			fprintf( stderr, "\n" );
		}
		fprintf( stderr, "\n" );
	}
};
struct Decision {
	int sx, sy, tx, ty;
	Decision():sx(0),sy(0),tx(0),ty(0){}
	Decision( int sx, int sy, int tx, int ty ):sx(sx),sy(sy),tx(tx),ty(ty){}
};
struct Pos {
	int x, y;
	Pos(){}
	Pos( int x, int y ):x(x),y(y){}
};


int id;
Board cboard;
Board valid, valid_init, canstop, safezone, railway, strongholds;
int head[H][W], dest[H*W*5][2], last[H*W*5], etot;
int flag_pos[2][2];

inline void end() {
	std::cout << "END\n" << std::flush;
}
inline int chess( int v ) {
	if( v < 0 ) return -1;
	return v % Kindcnt;
}
inline int belong( int v ) {
	if( v < 0 ) return -1;
	return v >= Kindcnt;
}
void add_edge( int i, int j, int ii, int jj ) {
	etot++;
	last[etot] = head[i][j];
	dest[etot][0] = ii;
	dest[etot][1] = jj;
	head[i][j] = etot;
}
void adde( int i, int j, int ii, int jj ) {
	add_edge( i, j, ii, jj );
	add_edge( ii, jj, i, j );
}
int dx[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
int dy[] = { 0, 0, -1, 1, -1, 1, -1, 1 };

void init() {
	for( int i = 0; i < 4; i++ ) adde( 1, i, 1, i+1 );
	for( int i = 0; i < 4; i++ ) adde( 5, i, 5, i+1 );
	for( int i = 0; i < 4; i++ ) adde( 11, i, 11, i+1 );
	for( int i = 0; i < 4; i++ ) adde( 15, i, 15, i+1 );
	for( int i = 1; i < 5; i++ ) adde( i, 0, i+1, 0 );
	for( int i = 1; i < 5; i++ ) adde( i, 4, i+1, 4 );
	for( int i = 11; i < 15; i++ ) adde( i, 0, i+1, 0 );
	for( int i = 11; i < 15; i++ ) adde( i, 4, i+1, 4 );
	adde( 5, 0, 6, 0 ); adde( 5, 2, 6, 2 ); adde( 5, 4, 6, 4 );
	adde( 10, 0, 11, 0 ); adde( 10, 2, 11, 2 ); adde( 10, 4, 11, 4 );
	adde( 6, 0, 6, 2 ); adde( 6, 2, 6, 4 );
	adde( 8, 0, 8, 2 ); adde( 8, 2, 8, 4 );
	adde( 10, 0, 10, 2 ); adde( 10, 2, 10, 4 );
	adde( 6, 0, 8, 0 ); adde( 8, 0, 10, 0 );
	adde( 6, 2, 8, 2 ); adde( 8, 2, 10, 2 );
	adde( 6, 4, 8, 4 ); adde( 8, 4, 10, 4 );
	//	valid 
	for( int i = 0; i <= 16; i++ )
		for( int j = 0; j <= 4; j++ )
			valid[i][j] = 1;
	valid[7][1] = valid[7][3] = 0;
	valid[9][1] = valid[9][3] = 0;
	//	canstop
	canstop = valid;
	canstop[10][1] = canstop[10][3] = 0;
	canstop[9][0] = canstop[9][2] = canstop[9][4] = 0;
	canstop[8][1] = canstop[8][3] = 0;
	canstop[7][0] = canstop[7][2] = canstop[7][4] = 0;
	canstop[6][1] = canstop[6][3] = 0;
	//	safezone
	safezone[2][1] = safezone[2][3] = safezone[3][2] = 1;
	safezone[4][1] = safezone[4][3] = 1;
	safezone[14][1] = safezone[14][3] = safezone[13][2] = 1;
	safezone[12][1] = safezone[12][3] = 1;
	//	valid_init
	valid_init = canstop;
	for( int i = 0; i <= 16; i++ )
		for( int j = 0; j <= 4; j++ )
			if( safezone[i][j] ) valid_init[i][j] = 0;
	//	railway
	railway = valid;
	for( int i = 2; i <= 4; i++ )
		for( int j = 1; j <= 3; j++ )
			railway[i][j] = 0;
	for( int i = 12; i <= 14; i++ )
		for( int j = 1; j <= 3; j++ )
			railway[i][j] = 0;
	for( int j = 0; j <= 4; j++ ) 
		railway[0][j] = railway[16][j] = 0;
	//	strongholds
	strongholds[0][1] = strongholds[0][3] = 1;
	strongholds[16][1] = strongholds[16][3] = 1;
}
/*
# 0 司令 * 1
# 1 军长 * 1
# 2 师长 * 2
# 3 旅长 * 2
# 4 团长 * 2
# 5 营长 * 2
# 6 连长 * 3
# 7 排长 * 3
# 8 工兵 * 3
# 9 地雷 * 3
# 10炸弹 * 2
# 11军旗 * 1 
*/
int kind_score[12][3] = { 
	{ 60 },					//# 0 司令 * 1
	{ 47 }, 				//# 1 军长 * 1
	{ 31, 26},   				//# 2 师长 * 2
	{ 23, 20},  				//# 3 旅长 * 2
	{ 17, 15},  				//# 4 团长 * 2
	{ 11, 9 },  				 //# 5 营长 * 2
	{ 7,  6, 5 },  				//# 6 连长 * 3
	{ 3, 3, 3 },   				//# 7 排长 * 3
	{ 42, 30, 20 },  				//# 8 工兵 * 3
	{ 21, 31, 41 },  				//# 9 地雷 * 3
	{ 44, 33 },  				//# 10炸弹 * 2
	{ 1000000 }			//# 11军旗 * 1 
};
int evaluate( const Board &board ) {
	int rt = 0;
	static int cnt[2][12];

	for( int c = 0; c < 2; c++ )
		for( int i = 0; i < 12; i++ ) 
			cnt[c][i] = 0;
	for( int i = 0; i <= 16; i++ )
		for( int j = 0; j < 5; j++ ) {
			if( board[i][j] != -2 ) {
				if( strongholds[i][j] ) continue;
				int v = board[i][j];
				int c = chess(v);
				int curid = belong(v);
				int dist = abs(i-flag_pos[curid^1][0]) + abs(j-flag_pos[curid^1][1]);
				int sgn = curid == id ? 1 : -1;
				int skind = kind_score[c][cnt[curid][c]];
				int sdist = 10 + (20-dist) * 10 / 20;
				cnt[curid][c]++;
				rt += sgn * skind * sdist;
			}
		}
	return rt;
}
bool isvalid( int i, int j ) {
	if( i < 0 || i > 16 ) return false;
	if( j < 0 || j > 4 ) return false;
	return valid[i][j];
}
/*
# kind 
# 0 司令 * 1
# 1 军长 * 1
# 2 师长 * 2
# 3 旅长 * 2
# 4 团长 * 2
# 5 营长 * 2
# 6 连长 * 3
# 7 排长 * 3
# 8 工兵 * 3
# 9 地雷 * 3
# 10炸弹 * 2
# 11军旗 * 1 
*/					   
int fight( int ca, int ka, int cb, int kb ) {
	if( ka == 10 || kb == 10 ) return -2;
	if( kb == 11 ) return ca * Kindcnt + ka;
	if( kb == 9 ) {
		if( ka == 8 ) 
			return ca * Kindcnt + ka;
		else
			return cb * Kindcnt + kb;
	} else {
		if( ka == kb ) return -2;
		else if( ka < kb ) return ca * Kindcnt + ka;
		else return cb * Kindcnt + kb;
	}
}
void next_board( Board &board, int x, int y, int xx, int yy ) {
	int &u = board[x][y];
	int &v = board[xx][yy];
	if( v == -2 ) {
		v = u;
		u = -2;
	} else {
		v = fight(belong(u),chess(u),belong(v),chess(v));
		u = -2;
	}
}
const char *hanzi( int v ) {
	if( v < 0 ) 
		return "  ";
	if( v == 0 ) 
		return "司";
	if( v == 1 ) 
		return "军";
	if( v == 2 ) 
		return "师";
	if( v == 3 ) 
		return "旅";
	if( v == 4 ) 
		return "团";
	if( v == 5 ) 
		return "营";
	if( v == 6 ) 
		return "连";
	if( v == 7 ) 
		return "排";
	if( v == 8 ) 
		return "兵";
	if( v == 9 ) 
		return "雷";
	if( v == 10 ) 
		return "炸";
	if( v == 11 ) 
		return "旗";
	return "";
}
int total_nodes;
void generate_next( const Board &board, 
		vector<Decision> &vc, int curid ) {
	bool has_flag = false;
	for( int i = 0; i <= 16; i++ )
		for( int j = 0; j < 5; j++ ) {
			int v = board[i][j];
			if( belong(v)==curid && chess(v) == 11 ) {
				has_flag = true;
				break;
			}
		}
	if( !has_flag ) return;
	for( int i = 0; i <= 16; i++ )
		for( int j = 0; j <= 4; j++ ) {
			int c = board[i][j];
			if( strongholds[i][j] ) continue;
			if( belong(c) == curid && chess(c) != 11 && chess(c) != 9 ) {
				if( railway[i][j] ) {
					queue<Pos> qu;
					Board vis;
					memset( &vis, 0, sizeof(vis) );
					qu.push( Pos(i,j) );
					vis[i][j] = 1;
					while( !qu.empty() ) {
						Pos u = qu.front();
						qu.pop();
						for( int t = head[u.x][u.y]; t; t = last[t] ) {
							int ii = dest[t][0], jj = dest[t][1];
							if( vis[ii][jj] ) continue;
							if( chess(c) != 8 && ((ii!=i) + (jj!=j) == 2) ) continue;
							vis[ii][jj] = 1;
							if( board[ii][jj] == -2 ) {
								vc.push_back( Decision(i,j,ii,jj)  );
								qu.push( Pos(ii,jj) );
							} else if( belong(board[ii][jj]) != curid ) {
								vc.push_back( Decision(i,j,ii,jj)  );
							}
						}
					}
					for( int d = 0; d < 4; d++ ) {
						int ii = i + dx[d], jj = j + dy[d];
						if( isvalid(ii,jj) && belong(board[ii][jj]) != curid ) {
							if( railway[ii][jj] ) continue;
							if( !safezone[ii][jj] || (safezone[ii][jj] && board[ii][jj]==-2) ) {
								vc.push_back( Decision(i,j,ii,jj)  );
							}
						}
					}
					for( int d = 4; d < 8; d++ ) {
						int ii = i + dx[d], jj = j + dy[d];
						if( isvalid(ii,jj) && belong(board[ii][jj]) != curid ) {
							if( !safezone[ii][jj] && !safezone[i][j] ) continue;
							if( railway[ii][jj] ) continue;
							if( !safezone[ii][jj] || (safezone[ii][jj] && board[ii][jj]==-2) ) {
								vc.push_back( Decision(i,j,ii,jj)  );
							}
						}
					}
				} else {
					for( int d = 0; d < 4; d++ ) {
						int ii = i + dx[d], jj = j + dy[d];
						if( isvalid(ii,jj) && belong(board[ii][jj]) != curid ) {
							if( !safezone[ii][jj] || (safezone[ii][jj] && board[ii][jj]==-2) ) {
								vc.push_back( Decision(i,j,ii,jj)  );
							}
						}
					}
					for( int d = 4; d < 8; d++ ) {
						int ii = i + dx[d], jj = j + dy[d];
						if( isvalid(ii,jj) && belong(board[ii][jj]) != curid ) {
							if( !safezone[ii][jj] && !safezone[i][j] ) continue;
							if( !safezone[ii][jj] || (safezone[ii][jj] && board[ii][jj]==-2) ) {
								vc.push_back( Decision(i,j,ii,jj)  );
							}
						}
					}
				}
			}
		}
}
Decision search( int maxdep, int curdep, Board &board, int &alpha, int &beta ) {
	vector<Decision> vc;
	total_nodes++;
	if( curdep == maxdep ) {
		if( (curdep&1) == 0 )
			alpha = max( alpha, evaluate(board) );
		else
			beta = min( beta, evaluate(board) );
		return Decision();
	}

	generate_next( board, vc, id ^ (curdep&1) );

	if( vc.empty() ) {
		return Decision();
	}

	Decision rt;
	for( int t = 0; t < (int)vc.size(); t++ ) {
		int aa = alpha, bb = beta;
		Decision &dec = vc[t];
		int sa = board[dec.sx][dec.sy];
		int sb = board[dec.tx][dec.ty];
		next_board( board, dec.sx, dec.sy, dec.tx, dec.ty );
		search( maxdep, curdep+1, board, aa, bb );
		board[dec.sx][dec.sy] = sa;
		board[dec.tx][dec.ty] = sb;
		if( (curdep & 1) == 0 ) {
			if( alpha < bb ) {
				alpha = bb;
				rt = vc[t];
				//} else if( alpha == bb ) {
				//	if( rand() & 1 ) rt = vc[t];
		}
		} else {
			if( beta > aa ) {
				beta = aa;
				rt = vc[t];
				//} else if( beta == aa ) {
				//	if( rand() & 1 ) rt = vc[t];
		}
		}
		if( alpha > beta ) return rt;
	}
	return rt;
}
void get_init() {
	int arr[2][25];
	fprintf( stderr, "get_init \n" );
	for( int i = 0; i < 2; i++ ) {
		for( int j = 0; j < 25; j++ ) {
			cin >> arr[i][j];
			cerr << arr[i][j] << " " ;
		}
		cerr << endl;
	}
	int cur;
	cur = 0;
	for( int i = 0; i <= 5; i++ )
		for( int j = 0; j < 5; j++ )
			if( valid_init[i][j] ) {
				cboard[i][j] = arr[0][cur++];
				if( chess(cboard[i][j]) == 11 ) {
					flag_pos[0][0] = i;
					flag_pos[0][1] = j;
				}
			}
	cur = 0;
	for( int i = 16; i >= 11; i-- )
		for( int j = 4; j >= 0; j-- )
			if( valid_init[i][j] )  {
				cboard[i][j] = arr[1][cur++] + Kindcnt;
				if( chess(cboard[i][j]) == 11 ) {
					flag_pos[1][0] = i;
					flag_pos[1][1] = j;
				}
			}

	for( int i = 0; i <= 16; i++ ) {
		for( int j = 0; j <= 4; j++ ) {
			if( cboard[i][j] < 0 )
				fprintf( stderr, "   " );
			else
				fprintf( stderr, "%s ", hanzi(chess(cboard[i][j])) );
		}
		fprintf( stderr, "\n" );
	}
	fprintf( stderr, "cboard:\n" );
	cboard.print();
}
void show_init() {
	int init[10][25] = { 
		{ 6 , 7 , 7 , 11 , 9 , 3 , 8 , 9 , 9 , 7 , 6 , 5 , 6 , 4 , 10 , 8 , 5 , 4 , 0 , 10 , 2 , 8 , 1 , 3 , 2 , },
		{ 4 , 7 , 9 , 11 , 9 , 0 , 8 , 5 , 9 , 7 , 1 , 8 , 6 , 10 , 5 , 4 , 3 , 3 , 6 , 10 , 2 , 8 , 6 , 7 , 2 , },
		{ 9 , 11 , 3 , 7 , 8 , 9 , 9 , 6 , 2 , 10 , 7 , 5 , 5 , 4 , 7 , 6 , 8 , 2 , 10 , 1 , 6 , 3 , 0 , 8 , 4 , },
		{ 9 , 11 , 9 , 7 , 6 , 3 , 8 , 8 , 7 , 9 , 5 , 7 , 5 , 4 , 10 , 10 , 4 , 2 , 6 , 3 , 6 , 0 , 1 , 8 , 2 , },
		{ 3 , 11 , 9 , 7 , 8 , 5 , 9 , 6 , 9 , 4 , 10 , 7 , 6 , 2 , 10 , 6 , 0 , 4 , 8 , 3 , 2 , 8 , 1 , 7 , 5 , },
		{ 8 , 9 , 5 , 11 , 9 , 3 , 10 , 3 , 6 , 9 , 4 , 6 , 5 , 2 , 7 , 10 , 2 , 0 , 6 , 7 , 1 , 8 , 8 , 7 , 4 , },
		{ 10 , 7 , 9 , 11 , 9 , 10 , 3 , 2 , 5 , 9 , 6 , 8 , 4 , 3 , 6 , 1 , 7 , 2 , 8 , 0 , 5 , 8 , 4 , 7 , 6 , },
		{ 9 , 11 , 9 , 7 , 5 , 9 , 5 , 2 , 10 , 3 , 7 , 6 , 3 , 2 , 10 , 8 , 6 , 6 , 8 , 4 , 4 , 7 , 1 , 8 , 0 , },
		{ 7 , 7 , 9 , 11 , 9 , 5 , 3 , 6 , 9 , 6 , 7 , 10 , 4 , 0 , 8 , 10 , 2 , 5 , 8 , 1 , 4 , 8 , 3 , 2 , 6 , },
		{ 9 , 11 , 9 , 7 , 7 , 1 , 6 , 3 , 9 , 10 , 10 , 8 , 3 , 4 , 5 , 5 , 2 , 2 , 6 , 0 , 6 , 7 , 8 , 8 , 4 , },
	};
	int i = 0;
	cerr << "show_init" << " ";
	for( int j = 0; j < 25; j++ ) {
		cout << init[i][j] << " ";
		cerr << init[i][j] << " ";
	}
	cout << endl;
	cerr << endl;
}
void change( int x, int y, int xx, int yy, int color, int kind ) {
	int tar = color * Kindcnt + kind;
	cboard[x][y] = -2;
	cboard[xx][yy] = tar;
}
void make_decision( int &sx, int &sy, int &tx, int &ty ) {
	int alpha = -oo, beta = oo;
	total_nodes = 0;
	Decision dec = search( 3, 0, cboard, alpha, beta );
	fprintf( stderr, "search %d nodes\n", total_nodes );
	sx = dec.sx;
	sy = dec.sy;
	tx = dec.tx;
	ty = dec.ty;
}
int main(int argc, char** argv) {
	unsigned seed = time(0);
	if (argc == 2) {
		cerr << "Excited! Get given seed = " << argv[1] << endl;
		seed = 0;
		for (char *pc = argv[1]; *pc; ++pc)
			seed = seed * 10 + (*pc - '0');
	}
	srand(seed);

	init();
	for (int i = 0; i < H; ++i) {
		for (int j = 0; j < W; ++j) {
			cboard[i][j] = -2;
		}
	}
	string op;
	int caseno = 0;
	int stepno = 1;
	while (true) {
		cin >> op;
		caseno++;
		fprintf( stderr, "caseno = %d\n", caseno );
		fprintf( stderr, "stepno = %d\n", stepno );
		if (op == "id") {
			cin >> id;
			cout << "idy004.3" << endl;
			end();
		} else if (op == "refresh") {
			get_init();
		} else if (op == "init") {
			show_init();
			end();
		} else if (op == "message") {
			int x, y, xx, yy, color, kind;
			cin >> x >> y >> xx >> yy >> color >> kind;
			fprintf( stderr, "idy002: message: %d %d %d %d %d\n", x, y, xx, yy, color*Kindcnt + kind );
			change( x, y, xx, yy, color, kind );
		} else if (op == "action") {
			int x, y, xx, yy;
			stepno+=2;
			make_decision(x, y, xx, yy);
			cout << x << " " << y << " " << xx << " " << yy << endl;
			fprintf( stderr, "idy002: action: (%d %d %s) -> (%d %d %s) \n", x, y, hanzi(chess(cboard[x][y])), xx, yy, hanzi(chess(cboard[xx][yy]))  );
			end();
		}
	}
}
