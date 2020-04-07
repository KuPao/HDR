## Recovering High Dynamic Range Radiance Maps from Photographs

**使用方法：**
將圖片放在同一個資料夾中，將圖檔名及曝光時間的倒數紀錄在 image_list.txt 中

```
DSC06930.JPG 500
DSC06929.JPG 250
DSC06928.JPG 125
DSC06927.JPG 60
...
```

使用Visual Studio編譯，指定資料夾的路徑

```bash
$ HDR.exe [image_dir]
```

程式會產生 radiance maps (hdr.hdr) 及使用 Reinhard tone-mapping 的結果 (tone.jpg)

**開發環境:**
Visual Studio 2019 on Windows10

**Library:**

1. [OpenCV](https://opencv.org)