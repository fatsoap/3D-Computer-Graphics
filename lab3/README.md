# 2021CG_Lab3_107502570

資工 4B*107502570*陳錕詮

## 程式說明

1. 一開始先讀指令操作檔案，獲得所有指令後放入全域變數。用拿到的視窗寬高開啟 glut 視窗。

2. 每次執行完 display 函數後會呼叫 glutPostRedisplay 函數，就會再次執行 display 函數。

3. display 函數

   - 會去看現在 screen 全域變數這個陣列，有哪些東西要畫，然後在畫之前用 polygon_clip 函數做裁切。

   - 會去執行 parseCommand 這個函數來執行指令操作。

4. parseCommand 函數

   - 每次執行一行指令。

   - 根據不同指令 handle 不同東西。

5. handleScale, handleRotate, handleTranslate 函數

   - 將全域變數的 3x3 陣列 "transformation_matrix" 做左乘運算。

   - 處理縮放、旋轉、位移

6. handleObject 函數

   - 讀取然後 parse 對應 object 的資料。

   - 將當前的 3x3 矩陣和 object 的資料丟入 objects 全域變數陣列

7. handleView & handleWM 函數

   - 處理 window-viewport-mapping 的東西

   - 將 vxl, vxr, vyb, vyt & 視窗寬高拿出來做 viewport 處理，算出 WM 矩陣。

8. handleObserver 函數

   - 處理 Eye Matrix 的東西

   - 先從 ex, ey, ez & coix, coiy, coiz 算出 GRM

   - 用 -ex, -ey, -ez 算出 Trans Martix, tilt 算出 Tilt Matrix

   - 以 Tilt Matrix _ Mirror Matrix _ GRM \* Trans Matrix 算出 EM

9. handlePM 函數

   - 藉由 yon, hither, hav, AR 算出 Projection Matrix

10. handleDisplay 函數

    - 清空之前畫面上的圖形

    - 跑 handlePM & handleWM 函數 算出需要的矩陣。

    - loop 所有 objects 將點對矩陣做運算。

    - 如果有 backfaces 的話要根據 object 的每個面去處理 backfaces

11. polygon_clip 函數

    - 根據 左下右上 的順序裁切圖形座標。

    - 需要時呼叫 getMid 函數 取得和 viewport 邊界相交的點。

12. handleReset 函數

    - transformation 陣列

13. handleComment 函數

    - 印出註釋

14. handleEnd 函數

    - 關掉視窗 & 結束程式

## Reference

1. 上課筆記

2. [ntu computer graphics](https://graphics.cmlab.csie.ntu.edu.tw/~robin/courses/3dcg08/)

3. [3D Viewing Pipeline & Types of Projection](https://www.youtube.com/watch?v=mXQRMKeX-8g)
