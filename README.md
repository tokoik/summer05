# summer05 - 今風 OpenGL の使い方（第８回 指標を用いた図形の描画）サンプルプログラム

## 1. 概要

このプログラムは、OpenGL の **頂点バッファオブジェクト (VBO)** と **指標（インデックスバッファ）** を用いて、幾何情報（頂点位置）と位相情報（頂点インデックス）を分離して GPU に転送し、`glDrawElements()` により 3 次元立方体のワイヤフレームを描画する手順を学ぶための、学生向けのサンプルプログラムです。本プログラムは、以下のブログ記事の解説に沿って学習を進めるための雛形として提供されています。

- [第８回 指標を用いた図形の描画](https://tokoik.github.io/blog/glsl/2009/09/09/glsl.html)

`glDrawArrays()` と異なり、頂点データを重複して定義することなくインデックス（`GL_ELEMENT_ARRAY_BUFFER`）を介して効率的に図形を描画する手法を学習します。

## 2. ビルド方法

このプログラムは [CMake](https://cmake.org/) を用いてビルド環境を整備します。各OSとも、ソースコードが置かれているディレクトリにターミナル（またはコマンドプロンプト）で移動してから、以下の手順を実行してください。なお、プログラムをビルドするためのバイナリディレクトリは、バージョン管理ファイル（.gitignore）の設定に合わせて **build** という名前にします。

### 2.1 Windows (Visual Studio 2022 の場合)

1. コマンドプロンプトまたは PowerShell を開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、CMake で構成を行います。

   ```bat
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022"
   ```

3. 生成された build フォルダ内の summer05.sln を Visual Studio で開きます。
4. ソリューションエクスプローラーで **summer05** プロジェクトを右クリックし、「スタートアップ プロジェクトに設定」を選択します。
5. 「ローカル Windows デバッガー」をクリックするか、F5 キーを押してビルドおよび実行します。

### 2.2 macOS (Xcode の場合)

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、Xcode 用のプロジェクトを生成します。

   ```sh
   mkdir build
   cd build
   cmake .. -G Xcode
   ```

3. 生成された build/summer05.xcodeproj を Xcode で開きます。
4. 左上のスキーム選択（再生ボタンの横）が **summer05** になっていることを確認します。
5. 「Run」ボタン（再生ボタン）をクリックするか、Command + R を押してビルドおよび実行します。

### 2.3 Ubuntu Linux

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 必要なパッケージ（freeglut3-dev など）がインストールされていることを確認し、以下のコマンドでビルドします。

   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

## 3. 使い方

### 3.1 プログラムの起動方法

- **Windows**: `build\Debug\summer05.exe`
- **macOS**: `open build/Debug/summer05.app` または Xcode 上で Run
- **Ubuntu Linux**: `cd build && ./summer05`

### 3.2 操作方法

- 3次元空間に配置された赤いワイヤフレーム立方体が透視投影変換されて表示されます。

## 4. 解説

### 4.1 インデックスバッファ (GL_ELEMENT_ARRAY_BUFFER) の設定

```cpp
/* 頂点座標バッファ (buffer[0]) の設定 */
glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
glBufferData(GL_ARRAY_BUFFER, sizeof (Position) * 8, NULL, GL_STATIC_DRAW);
position = (Position *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
/* 8頂点分の座標を書き込む */
glUnmapBuffer(GL_ARRAY_BUFFER);
glBindBuffer(GL_ARRAY_BUFFER, 0);

/* インデックスバッファ (buffer[1]) の設定 */
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (Edge) * 12, NULL, GL_STATIC_DRAW);
edge = (Edge *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
/* 12稜線分のインデックス (24頂点インデックス) を書き込む */
glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
```

### 4.2 glDrawElements による描画

```cpp
/* 頂点座標バッファのバインド */
glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

/* インデックスバッファのバインド */
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);

/* インデックスを用いて描画 */
glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
```
