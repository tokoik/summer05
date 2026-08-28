#if defined(_WIN32)
#  define _USE_MATH_DEFINES
#  define _CRT_SECURE_NO_WARNINGS
#  include <GL/glew.h>
#  include <GL/glut.h>
#elif defined(__APPLE__)
#  define GL_SILENCE_DEPRECATION
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>

/*
** シェーダのソースプログラムの読み込みに使う関数
*/
extern int readShaderSource(GLuint shader, const char* file);
extern void printShaderInfoLog(GLuint shader);
extern void printProgramInfoLog(GLuint program);

/*
** シェーダオブジェクト
*/
static GLuint vertShader;
static GLuint fragShader;
static GLuint gl2Program;

/*
** 直交投影変換行列を求める
*/
extern void orthogonalMatrix(float left, float right,
  float bottom, float top,
  float zNear, float zFar,
  GLfloat* matrix);

/*
** 透視投影変換行列を求める
*/
extern void perspectiveMatrix(float left, float right,
  float bottom, float top,
  float zNear, float zFar,
  GLfloat* matrix);

/*
** 画角から透視投影変換行列を求める
*/
extern void cameraMatrix(float fovy, float aspect,
  float zNear, float zFar,
  GLfloat* matrix);

/*
** 視野変換行列を求める
*/
extern void lookAt(float ex, float ey, float ez,
  float tx, float ty, float tz,
  float ux, float uy, float uz,
  GLfloat* matrix);

/*
** 行列の積を求める
*/
extern void multiplyMatrix(const GLfloat* m0,
  const GLfloat* m1,
  GLfloat* matrix);

/*
** 投影変換行列
*/
static GLfloat projectionMatrix[16];

/*
** 投影変換行列の uniform 変数の場所
*/
static GLint projectionMatrixLocation;

/*
** attribute 変数 position の頂点バッファオブジェクト
*/
static GLuint buffer[2];

/*
** 画面表示
*/
static void display(void)
{
  /* 画面クリア */
  glClear(GL_COLOR_BUFFER_BIT);

  /* シェーダプログラムを適用する */
  glUseProgram(gl2Program);

  /* 投影変換行列の uniform 変数 projectionMatrix に変換行列の値を設定する */
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, projectionMatrix);

  /* 頂点バッファオブジェクトとして buffer[0] を指定する */
  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

  /* 頂点バッファオブジェクトの指標として buffer[1] を指定する */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);

  /* 図形を描く */
  glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

  /* 頂点バッファオブジェクトを解放する */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glFlush();
}

/*
** 初期化
*/
static void init(void)
{
  /* シェーダプログラムのコンパイル／リンク結果を得る変数 */
  GLint compiled, linked;

#if defined(_WIN32)
  /* GLEW の初期化 */
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    exit(1);
  }
#endif

  /* 背景色 */
  glClearColor(1.0, 1.0, 1.0, 1.0);

  /* シェーダオブジェクトの作成 */
  vertShader = glCreateShader(GL_VERTEX_SHADER);
  fragShader = glCreateShader(GL_FRAGMENT_SHADER);

  /* シェーダのソースプログラムの読み込み */
  if (readShaderSource(vertShader, "simple.vert")) exit(1);
  if (readShaderSource(fragShader, "simple.frag")) exit(1);

  /* バーテックスシェーダのソースプログラムのコンパイル */
  glCompileShader(vertShader);
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(vertShader);
  if (compiled == GL_FALSE) {
    fprintf(stderr, "Compile error in vertex shader.\n");
    exit(1);
  }

  /* フラグメントシェーダのソースプログラムのコンパイル */
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(fragShader);
  if (compiled == GL_FALSE) {
    fprintf(stderr, "Compile error in fragment shader.\n");
    exit(1);
  }

  /* プログラムオブジェクトの作成 */
  gl2Program = glCreateProgram();

  /* シェーダオブジェクトのシェーダプログラムへの登録 */
  glAttachShader(gl2Program, vertShader);
  glAttachShader(gl2Program, fragShader);

  /* シェーダオブジェクトの削除 */
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);

  /* attribute 変数 position の index を 0 に指定する */
  glBindAttribLocation(gl2Program, 0, "position");

  /* シェーダプログラムのリンク */
  glLinkProgram(gl2Program);
  glGetProgramiv(gl2Program, GL_LINK_STATUS, &linked);
  printProgramInfoLog(gl2Program);
  if (linked == GL_FALSE) {
    fprintf(stderr, "Link error.\n");
    exit(1);
  }

  /* index が 0 の attribute 変数を有効にする */
  glEnableVertexAttribArray(0);

  /* 透視投影変換行列を求める */
  GLfloat perspective[16];
  cameraMatrix(30.0f, 1.0f, 7.0f, 11.0f, perspective);

  /* 視野変換行列を求める */
  GLfloat viewing[16];
  lookAt(4.0f, 5.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, viewing);

  /* 視野変換行列と投影変換行列の積を projectionMatrix に入れる */
  multiplyMatrix(viewing, perspective, projectionMatrix);

  /* uniform 変数 projectionMatrix の場所を得る */
  projectionMatrixLocation = glGetUniformLocation(gl2Program, "projectionMatrix");

  /* 頂点バッファオブジェクトを２つ作る */
  glGenBuffers(2, buffer);

  /* 頂点のデータ型 */
  typedef GLfloat Position[3];

  /* 頂点バッファオブジェクトに８頂点分のメモリ領域を確保する */
  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Position) * 8, NULL, GL_STATIC_DRAW);

  /* 頂点バッファオブジェクトのメモリをプログラムのメモリ空間にマップする */
  Position* position = (Position*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  /* 頂点バッファオブジェクトのメモリにデータを書き込む */
  position[0][0] = -1.0f;
  position[0][1] = -1.0f;
  position[0][2] = -1.0f;

  position[1][0] = 1.0f;
  position[1][1] = -1.0f;
  position[1][2] = -1.0f;

  position[2][0] = 1.0f;
  position[2][1] = -1.0f;
  position[2][2] = 1.0f;

  position[3][0] = -1.0f;
  position[3][1] = -1.0f;
  position[3][2] = 1.0f;

  position[4][0] = -1.0f;
  position[4][1] = 1.0f;
  position[4][2] = -1.0f;

  position[5][0] = 1.0f;
  position[5][1] = 1.0f;
  position[5][2] = -1.0f;

  position[6][0] = 1.0f;
  position[6][1] = 1.0f;
  position[6][2] = 1.0f;

  position[7][0] = -1.0f;
  position[7][1] = 1.0f;
  position[7][2] = 1.0f;

  /* 頂点バッファオブジェクトのメモリをプログラムのメモリ空間から切り離す */
  glUnmapBuffer(GL_ARRAY_BUFFER);

  /* index が 0 の attribute 変数に頂点バッファオブジェクトの場所と書式を設定する */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  /* 頂点バッファオブジェクトを解放する */
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  /* 稜線のデータ型 */
  typedef GLuint Edge[2];

  /* 頂点バッファオブジェクトに１２稜線分のメモリ領域を確保する */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Edge) * 12, NULL, GL_STATIC_DRAW);

  Edge* edge = (Edge*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

  /* 頂点バッファオブジェクトのメモリにデータを書き込む */
  edge[0][0] = 0;
  edge[0][1] = 1;

  edge[1][0] = 1;
  edge[1][1] = 2;

  edge[2][0] = 2;
  edge[2][1] = 3;

  edge[3][0] = 3;
  edge[3][1] = 0;

  edge[4][0] = 0;
  edge[4][1] = 4;

  edge[5][0] = 1;
  edge[5][1] = 5;

  edge[6][0] = 2;
  edge[6][1] = 6;

  edge[7][0] = 3;
  edge[7][1] = 7;

  edge[8][0] = 4;
  edge[8][1] = 5;

  edge[9][0] = 5;
  edge[9][1] = 6;

  edge[10][0] = 6;
  edge[10][1] = 7;

  edge[11][0] = 7;
  edge[11][1] = 4;

  /* 頂点バッファオブジェクトのメモリをプログラムのメモリ空間から切り離す */
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

  /* 頂点バッファオブジェクトを解放する */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*
** メインプログラム
*/
int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  init();
  glutMainLoop();

  return 0;
}
