# 调用webapi的车牌识别
## 本项目可用于替代任何车牌识别，比如easypr等，缺点是需要联网，不过配置简单，一共只需要四步

1、对vs的配置：

   项目->属性->连接器->输入->附加依赖项里加入ws2_32.lib
   
   项目->属性->C/C++->语言->符合模式：选择否
	 
2、将第28-220行代码复制到需要车牌识别的文件中，注意头文件需要按需添加，没有的要补上

3、将main函数中所有代码复制到需要车牌识别的函数中

4、改动240行代码调用sendPic函数的地方：

第1 2个参数不用动，第三个参数为要识别的图片路径，第四个参数为要识别的图片名，第五个参数不要动，第六个参数为api_user，第七个参数为api_key，为保护接口安全，最后两个接口需要联系我，每人拿到的user和key都不一样，需要key才接口可以正常工作
