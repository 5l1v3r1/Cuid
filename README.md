
# Cuid 
# Avoiding Detection/Hidden Process's & System wide changes with .mui file's. 
# Tech
mui files are are application extentions, that serve's as a local data base for a dll or an executable,<br> 
despite the fact that those are not executable files they, at a big extent influance existing application's activety.<br> 
One biggie for any spyware/adware is avoiding detection, while any respectfull rootkit would operate from ring 0, To hide its existance at an infected machine.<br> 
it is very well common for rootkits & Rats to change Critical Structures at NtosKrnl for hiding one existance from the user operating the target machine.<br> 
 and in this Repo i would concentrate at disabling Ci, while hiding that from the user, but the extent of changing mui files to "customize" the Ui, can go much further (and would not requere the machine to be 'safeboot disabled') as this poc do.
Well, as i have witness, this may be absolute, as there is an easyier work-around.<br> 
while Code Integrity (S.T CI (ci.dll)) was introduced as far back from vista, it is Only a Security feature That Can be bypassed by simply running: <br>
bcdedit /set testsigning on<br>
but guess what, you got a big sign infront of the user saying (TEST MODE), well we didnt do that much if so, cuz the opertunity to control the target system from ring 0 (as i see it) will allow you to better hide your activty (cuz afaik if your a good enof coder there is almost nothing you cannot implement as a spyware go's with user mode and priv).<br>
So what a great thing could be for a malware author to just remove this sign (i am aware that there are different ways to disable Ci w/o that but that requeres some pg manipulation, and i will focus on this finding alone in this repo.).<br>
# Well looks like MS got it all figured Out... (mmmmm)  not really.
like almost everything at the windows os it all comes down to a few veriables sitting at some exe (Ntos) or dll memory, or evan far better a simple text file (Mui).<br>
looking at shell32.dll.mui (well shell 32 hold almost every aspect of the users interface, it is the "shell" wrapper, so this is the first place i have go to in \system32\en-US\...) with <html><a href="http://www.angusj.com/resourcehacker/">resHacker</a></html>
![](PoC/ResHacker.png)
you can see many interesting variables (you can with notepad also...), guess what.... "TestMode" simply sits there as a veriable. well, very easy, if test signing is on then when shell32.dll is initialized it goes to table 2069 (not really depand on the versions but this do not make any differance as you will see further on). 
# Well what will heapen if we simply delete "TestMode"?????
well the answer is nothing, well (besides the fact that the sign is now off), but hey guess what, in System32\SysWow64\\..\\your leng...<br>
there are ton's of other interesting dll.mui files like Taskmgr.exe.mui ...  UserDataService.dll.mui and the list goe's on and on...<br>
enough talking, you may think, well but the shell is using that mui file to diplay data, well it goe's as far as a logout to make it use your modified .mui file (you can modify it and wait to the user logoff programaticly) and it will be initialized again with your 'payload' i cant evan call it that....<br>
So i have wrote a simple C program that illustrates hiding this Very Critical Change to the system.. but you can experiment on yourself and make other changes.
i myself, adjusted shell32.dll.mui & basebrd.dll.mui and coded them into Binary (inside the 'Build' zip directory as def.h (its a 1.5 mb file so i have zipped it for you)), and implemented a very simple 'mui exchanger' as a proof of consept it will requere the user to log off (or a reboot by the program (cuz i enable the testSigning in this poc)), but afaik if per say TaskMgr is not working at the givven moment then you can simplly change the mui, w/o any user need to log off or restart the pc, as i sayed there is much potential at making changes to this simple formated file's as far as dfir, and malware usage.

you can compile my code and test it (its win32 so youll need to adjust to your operating system (sysnative vs system32)) but there is not much else to be done. Or if you do not trust me simplly delete the "TestMode" or any other var you dont want the user to see (can be warning's or what ever the fk you want). and run it by your self. you can evan experiment more.

well.. to build from my source you will need to download the build part, (i have attached a 2 modified .mui file's if you dont trust my binarys.....), or modify them your self (id recommend with reshaker cuz simply deleting the value by hand will curropt the file format leading to a system fkUp,).<br>
# Important!!!
if you do compile my source and run it i will recomand to back up your \windows\branding\basebrd\en-US\basebrd.dll.mui & \Sysnative\en-Us\shell32.dll.mui before running this, so you can recover the system if anything go's wrong (i have tested this only at win 10 from early builds up to 16232..) but you can never know...

# Well here is my PoC, as you can see im obviously running w/o Ci, but there is no sign to it....
![](PoC/Poc.JPG)
# Muahahaha Enjoy!!
#


