# jsuse
极优 C++ 库

Windows 平台的通用 C++ 库

已配置为静态库

字符集使用 Unicode 字符集，不支持多字节字符。

“C++ > 代码生成 > 运行库” 配置为静态链接，MT(release) 和 MTd(debug)，引用的工程一般也必须设置为静态链接。

工程使用 VS 2008 SP1 编译，在 jucpp 目录的父级目录的 lib 文件夹下生成 6 个静态库文件: cryptlib.lib cryptlibD.lib jubase.lib jubaseD.lib juwnd.lib juwndD.lib。尾部带 D 的库文件是 Debug 版本。

项目 cryptlib 是开源 cryptopp 源码，没有任何修改。

jubase 项目是基本库，不含窗口部分，但是包含绘图组件。

juwnd 是窗口库，包含 Layout 组件，对话框等组件。
