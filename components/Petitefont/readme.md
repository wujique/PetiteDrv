#### 源文件说明

1. gb2312.h和gb18030.h 从zint库拷贝过来的，里面是GBK和UNICODE的映射表。

2. conv.c，这里是编码转换。比如将utf_8转GBK。

3. font_6x12.c, font_8x8.c,font_8x16.c,这三个ASC点阵，是从tslib里面拷贝来的，其根源实际在linux。不过，这三个点阵，大小写的基线并不对齐。在LCD上显示出来的效果其实并不好。

4. font_asc_6x12.c，这是屋脊雀工作室在zimo软件上人工描点的ASC字体。本字体考虑了大小写的基线。并且考虑了防止误认问题，比如数字0和字母o的区别。欢迎使用优化。

5. font_unicode.c，这是屋脊雀工作室的按需裁剪字库算法。按需裁剪字库，也就是使用freetype从指定矢量字库中提取指定字符的bitmap和dsc信息。通常用在固定显示内容，存储空间较小的项目。指定字符可以包含ASC、中文、日文、韩文、法文等等。 font/font_file.bin是一个示例字库，包含多个国家的“确认”。

   ```
   /*登录,登錄,Login,Login,로그인,Anmeldung,S'identifier,Accesso,Iniciar sesión,Đăng nhập,Логин,Entrar,Masuk,Oturum aç,เข้าสู่ระบบ*/
   ```

6. font_wjq.c，这是屋脊雀工作室使用freetype转换的GBK18030全字库算法。具体请见font目录。

7. font_ymy.c，这是 易沐雨软件转换得到的点阵算法。

8. 所有字库都参考lvgl font的格式。

9. 想要做好显示功能，请看图bitmap.jpg，特别是做多国语言。

   ![font_dis](.\pic\bitmap.jpg)

10. bilibili有视频聊字库的。搜索屋脊雀即可找到。

11. **屋脊雀提供有偿服务，如有需求，欢迎联系。咨询200元/小时，裁剪字库制作（含使用指导）按工作量报价，1600元/每人天**。

#### 联系方式

 bilibili、taobao搜索：屋脊雀。

www.wujique.com

#### COG显示示例

如下图，第一行是屋脊雀制作的中文GBK1212点阵加上ASC 1206点阵。ASC点阵大小写基线对齐。

第二、第三、第四行则是屋脊雀工作室按需提取的点阵，测试字库仅包含“登录”的多国语言点阵。

在1212的点阵中，泰语显示并不理想，因为泰语字符可能有上下标，字形非常高。

第五行是tslib中的VGA点阵，大小写基线不对齐，效果不好。

![font_dis](.\pic\font_dis.jpg)

