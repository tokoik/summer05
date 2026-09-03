# summer05 - 今風 OpenGL の使い方（第８回 指標を用いた図形の描画）サンプルプログラム

## 1. 概要

このプログラムは、OpenGL の **頂点バッファオブジェクト (VBO)** と **指標（インデックスバッファ）** を用いて、幾何情報（頂点位置）と位相情報（頂点インデックス）を分離して GPU に転送し、`glDrawElements()` により 3 次元立方体のワイヤフレームを描画する手順を学ぶための、学生向けのサンプルプログラムです。本プログラムは、以下のブログ記事の解説に沿って学習を進めるための雛形として提供されています。

- [第８回 指標を用いた図形の描画](https://tokoik.github.io/blog/2009-09-09.html)

`glDrawArrays()` と異なり、頂点データを重複して定義することなくインデックス（`GL_ELEMENT_ARRAY_BUFFER`）を介して効率的に図形を描画する手法を学習します。

## 2. 対応環境

- **Windows**: Windows 10 / 11, Visual Studio 2022 (MSVC C++17)
- **macOS**: macOS 12 Monterey 以降, Xcode 14 以降 / Command Line Tools
- **Linux**: Ubuntu 22.04 LTS 以降, GCC / Clang (C++17 対応コンパイラ)
- **ビルドツール**: CMake 3.22 以降

## 3. ビルド手順

このプログラムは [CMake](https://cmake.org/) を用いてビルド環境を整備します。各 OS とも、ソースコードが置かれているディレクトリにターミナル（またはコマンドプロンプト）で移動してから、以下の手順を実行してください。なお、プログラムをビルドするためのバイナリディレクトリは、バージョン管理ファイル（.gitignore）の設定に合わせて **build** という名前にします。

> cmake-gui で設定することも可能です。その際は、`Source code path` にはプロジェクトのフォルダを指定し、`Build path` にはプロジェクトのフォルダの中に作った build というフォルダを指定してください。その後、`Configure` → `Generate` の順にクリックした後、`Open Project` をクリックすれば、開発環境が起動します。

### 3.1 Windows (Visual Studio 2022 の場合)

1. コマンドプロンプトまたは PowerShell を開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、CMake で構成を行います。

   ```bat
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022"
   ```

3. 生成された build フォルダ内の `summer05.sln` を Visual Studio で開きます。
4. ソリューションエクスプローラーで **summer05** プロジェクトを右クリックし、「スタートアップ プロジェクトに設定」を選択します。
5. 「ローカル Windows デバッガー」をクリックするか、F5 キーを押してビルドおよび実行します。

### 3.2 macOS (Xcode の場合)

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、Xcode 用のプロジェクトを生成します。

   ```sh
   mkdir build
   cd build
   cmake .. -G Xcode
   ```

3. 生成された `build/summer05.xcodeproj` を Xcode で開きます。
4. 左上のスキーム選択（再生ボタンの横）が **summer05** になっていることを確認します。
5. 「Run」ボタン（再生ボタン）をクリックするか、Command + R を押してビルドおよび実行します。

### 3.3 Ubuntu Linux

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 必要なパッケージ（freeglut3-dev など）がインストールされていることを確認し、以下のコマンドでビルドします。

   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

## 4. 起動方法

各 OS とも、ビルド後に生成されるバイナリディレクトリ (build) やそのサブフォルダから起動します。

- **Windows**

  Visual Studio 上で「ローカル Windows デバッガー」をクリックして実行するか、またはコマンドプロンプトから以下のコマンドで起動します。

  ```cmd
  cd build\Debug
  summer05.exe
  ```

- **macOS**

  Xcode 上で左上の「Run（再生ボタン）」をクリックして実行します。アプリケーションバンドルを直接起動する場合は、Finder から `build/Debug/summer05.app` を開くか、ターミナルから `open build/Debug/summer05.app` を実行します。

- **Ubuntu Linux**

  ターミナルから以下のコマンドで実行ファイル（バイナリ）を直接起動します。

  ```sh
  cd build
  ./summer05
  ```

## 5. 操作方法

- 3次元空間に配置された赤いワイヤフレーム立方体が透視投影変換されて表示されます。

## 6. プログラムの解説

### 6.1 立方体の頂点データとインデックス生成 (wireCube)

`cpp
GLuint wireCube(const GLuint* buffer)
{
  /* 頂点のデータ型 */
  typedef GLfloat Position[3];

  /* 頂点バッファオブジェクトに８頂点分のメモリ領域を確保する */
  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Position) * 8, NULL, GL_STATIC_DRAW);

  /* 頂点バッファオブジェクトのメモリをプログラムのメモリ空間にマップする */
  Position* position = (Position*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  /* 頂点データを position に書き込む (8頂点分) */
  ...

  /* 頂点バッファオブジェクトのメモリをプログラムのメモリ空間から切り離す */
  glUnmapBuffer(GL_ARRAY_BUFFER);

  /* 頂点バッファオブジェクトを解放する */
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  /* 稜線のデータ型 */
  typedef GLuint Edge[2];

  /* 頂点バッファオブジェクトに１２稜線分のメモリ領域を確保する */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Edge) * 12, NULL, GL_STATIC_DRAW);

  /* 頂点バッファオブジェクトのメモリをプログラムのメモリ空間にマップする */
  Edge* edge = (Edge*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

  /* 稜線データを edge に書き込む (12稜線分) */
  ...

  /* 頂点バッファオブジェクトのメモリをプログラムのメモリ空間から切り離す */
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

  /* 頂点バッファオブジェクトを解放する */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return 24;
}
`

### 6.2 インデックスバッファを用いた描画 (display)

`cpp
/* 画面クリア */
glClear(GL_COLOR_BUFFER_BIT);

/* プログラムオブジェクトを適用する */
glUseProgram(gl2Program);

/* 投影変換行列の uniform 変数 projectionMatrix に変換行列の値を設定する */
glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, projectionMatrix);

/* 頂点バッファオブジェクトとして buffer[0] を指定する */
glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

/* index が 0 の attribute 変数を有効にする */
glEnableVertexAttribArray(0);

/* index が 0 の attribute 変数に頂点バッファオブジェクトの場所と書式を設定する */
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

/* 頂点バッファオブジェクトの指標として buffer[1] を指定する */
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);

/* 図形を描く */
glDrawElements(GL_LINES, points, GL_UNSIGNED_INT, 0);

/* index が 0 の attribute 変数を無効にする */
glDisableVertexAttribArray(0);

/* 頂点バッファオブジェクトを解放する */
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
glBindBuffer(GL_ARRAY_BUFFER, 0);

/* 固定機能に戻す */
glUseProgram(0);

glFlush();
`
