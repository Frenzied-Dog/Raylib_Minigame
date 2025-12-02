# 🎮 專案名稱：Raylib 迷你遊戲合集 (Raylib Minigame Collection)

## 🌟 專案概述

這是一個使用 **raylib** 輕量級遊戲庫開發的迷你遊戲合集 。專案旨在展示 raylib 的強大功能，並提供一個簡單且可擴展的框架，用於開發和整合多個獨立的小遊戲。

## 🚀 專案特色

  * **多平台支援：** 同時支援 Windows、macOS 和 Linux (Ubuntu) 作業系統。
  * **Debug/Release 模式：** 支援包含偵錯資訊的 `debug` 模式和經過優化 (`-O2`) 的 `release` 模式 。
  * **簡潔的遊戲框架：** 專案結構清晰，易於添加新的小遊戲模組。
  * **raylib 驅動：** 利用 raylib 的簡單 API 實現高效且快速的遊戲開發。

## 🛠️ 開發環境設定

本專案使用 C 語言開發，並依賴 **raylib** 遊戲庫 。

### 必備條件

1.  **C 編譯器** (Windows: MinGW/GCC , macOS: Clang , Linux: GCC )。
2.  **raylib 庫**：您需要將 raylib 的標頭檔放在專案的 `include/` 目錄，並將靜態庫檔案放在對應平台的 `lib/<platform>` (如 `lib/win64` ) 目錄下。

### 📦 專案檔案結構 (輸出路徑)

編譯後的執行檔會輸出到專案根目錄下的 `build/` 資料夾 。

  * **Debug 版本輸出：** `build/debug/` 
  * **Release 版本輸出：** `build/release/` 

-----

## 💻 如何構建和執行

提供兩種構建方式：推薦使用 `make` 工具，或直接使用 CLI 命令。

### 方式一：使用 Make (推薦)

專案已包含 `Makefile`，可自動處理平台判斷、編譯器選取和庫鏈接。

#### 1\. 構建指令

在終端機中，切換到專案根目錄並執行以下命令：

| 模式             | 指令                   | 描述                                                 |
| :--------------- | :--------------------- | :--------------------------------------------------- |
| **Debug 構建**   | `make debug` 或 `make` | 預設模式，用於開發和偵錯。                           |
| **Release 構建** | `make release`         | 經 `-O2` 優化，用於發佈。                            |
| **清理**         | `make clean`           | 清除 `build/debug` 和 `build/release` 目錄下的檔案。 |

#### 2\. 執行指令

構建完成後，可使用 `make run` 快速執行 **Debug 版本** ：

```bash
make run
```

### 方式二：不使用 Make (使用 CLI)

如果您的環境沒有安裝 `make`，您可以根據您的作業系統和需要的模式，手動執行編譯器命令。

> **注意：** 這些命令直接基於 `Makefile` 內容，您可能需要根據您的系統調整編譯器路徑 (例如 Windows 的 `C:/MinGW64/bin/gcc.exe` )。

#### 🔹 變數定義

| 變數             | Debug 模式                                                         | Release 模式                                                        |
| :--------------- | :----------------------------------------------------------------- | :------------------------------------------------------------------ |
| **通用旗標**     | `-g -fdiagnostics-color=always -I"include"`  | `-O2 -fdiagnostics-color=always -I"include"`  |
| **Debug 輸出**   | `build/debug/main_<platform>`                                      | N/A                                                                 |
| **Release 輸出** | N/A                                                                | `build/release/main_<platform>`                                     |

#### 🔹 Windows (64-bit, 使用 MinGW)

| 模式        | 編譯器  | 指令範例 (單行)                                                                                                                                                        |
| :---------- | :---------------------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Debug**   | `C:/MinGW64/bin/gcc.exe`      | `C:/MinGW64/bin/gcc.exe -g -fdiagnostics-color=always -I"include" src/*.c -o build/debug/main.exe -L"lib/win64" -lraylib -lgdi32 -lwinmm`        |
| **Release** | `C:/MinGW64/bin/gcc.exe`      | `C:/MinGW64/bin/gcc.exe -O2 -fdiagnostics-color=always -I"include" src/*.c -o build/release/main.exe -L"lib/win64" -lraylib -lgdi32 -lwinmm`  |

#### 🔹 macOS (使用 Clang/GCC)

| 模式        | 編譯器  | 指令範例 (單行)                                                                                                                                                                                              |
| :---------- | :---------------------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Debug**   | `/usr/bin/clang`              | `/usr/bin/clang -g -fdiagnostics-color=always -I"include" src/*.c -o build/debug/main_mac -L"lib/mac" -lraylib -framework OpenGL -framework OpenAL -framework Cocoa -framework IOKit`  |
| **Release** | `/usr/bin/clang`              | `/usr/bin/clang -O2 -fdiagnostics-color=always -I"include" src/*.c -o build/release/main -L"lib/mac" -lraylib -framework OpenGL -framework OpenAL -framework Cocoa -framework IOKit`   |

#### 🔹 Linux (使用 GCC)

| 模式        | 編譯器  | 指令範例 (單行)                                                                                                                                                             |
| :---------- | :---------------------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Debug**   | `/usr/bin/gcc`                | `/usr/bin/gcc -g -fdiagnostics-color=always -I"include" src/*.c -o build/debug/main_linux -L"lib/linux" -lraylib -lGL -lm -ldl -lrt -lpthread -lX11`  |
| **Release** | `/usr/bin/gcc`                | `/usr/bin/gcc -O2 -fdiagnostics-color=always -I"include" src/*.c -o build/release/main -L"lib/linux" -lraylib -lGL -lm -ldl -lrt -lpthread -lX11`     |

-----

## 🤝 貢獻 (Contribution)

歡迎所有類型的貢獻！如果您想添加一個新的小遊戲，請提交 Pull Request。