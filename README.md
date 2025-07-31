## CS 144: Introduction to Computer Networking

## 前言

​	现在是暑假，本🉑在被驾照考试折磨的情况下学习这门计网课程😭。这是我第一次系统接触计算机网络的知识，只能说最起码认识了该领域的一些名词，他们深层次的机制还没有完全掌握

## 课程感受

​	就lecture video来言，我觉得还可以吧，其实就是若干个小视频拼接起来，之间的关联有但是不是很强，另外讲的也没有感觉特别有趣或者给人恍然大悟的感觉，感觉下来就是一般

​	对于lab，**lab整体的设计还挺不错的**，不过确实是**代码量小一点**，然后主要就是**实现接口**，正如网友们所说：自主设计的范围不大，~~但是对我这种菜鸡还是有点友好，尽管我也没有自己去写，但是也方便我学习大佬的代码，理解流程~~。不过有一说一，最起码通过这门课的lab，倒是让我更加熟悉**C++语法**了，比如stl的容器、迭代器、optional、面向对象设计这些，感觉还是很不错的。另外这门课的lab_handout有点谜语人，非常建议结合代码

## Lab

​	我没有独立实现这门课的lab，所以我的lab都是学习大佬的，一开始我参考了[b站阿苏EEer](https://www.bilibili.com/video/BV1v14y1s7oq?t=9.8)的lab0-4，但是最后没有通过所有测试，然后学习了[kiprey](https://kiprey.github.io/tags/CS144/)的lab5-7，最后想着干脆把所有测试都通过吧，也就根据[kiprey](https://kiprey.github.io/tags/CS144/)的代码重新code了一下lab0-4，**所以想要参考代码的话，可以直接到[kiprey](https://kiprey.github.io/tags/CS144/)的博客哦**，另外在参考的同时，我还发现这位学长的**代码风格很不错** 😋

环境：GNU/Linux

我的做法是在mac下使用pd虚拟机，pd虚拟机可以共享宿主机文件，修改同步超级方便，只需要把虚拟机挂在后台然后ssh，然后输入命令就可以正常使用Linux环境了

​	下面梳理一下这些lab都在干什么

🧪Lab0 -- 创建一个双向字节流

🧪Lab1 -- 构建一个字节重组器

🧪Lab2 -- 在前两个lab的基础上构建TCPReceiver	

🧪Lab3 -- 构建TCPSender

🧪Lab4 -- 在前面的基础上整体构建一个TCPConnection

🧪Lab5 -- 构建一个网络接口 network interface（也被称为适配器） 也就是模拟ARP协议

🧪Lab6 -- 模拟一个router路由器

🧪Lab7 -- 无需代码，操作server和client的连接

​	lab工作流：

​	CS144的lab代码仓库会随学期清空，你可以fork别人的实现然后回退到初始版本（当然你也可以直接用我的仓库然后回退）

​	第一次拿到lab的时候，在本仓库下

```bash
mkdir build
cd build
cmake ..
```

​	此后只需要在`build`目录下工作即可，包括`make` ，debug等

​	如果拿到干净的lab发现make报错，可以将错误信息喂给gpt

## 结语

​	😭灰溜溜去读《自顶向下》了，run～

