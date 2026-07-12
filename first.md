# first md file

## I got following recommendation after I created my first git repository
```powershell
echo "# testrepo" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin https://github.com/yanghava/testrepo.git
git push -u origin main

## branch
the first and default branch is named master

```powershell 
PS D:\codes\ProgrammerLearning\testrepo> git branch
\* master

### change branch name

using "git branch -M" to rename (move) the branch 
```powershell
PS D:\codes\ProgrammerLearning\testrepo> git branch -M main
PS D:\codes\ProgrammerLearning\testrepo> git branch
\* main

### creat a new branch
git branch <branch-name>
```powershell
PS D:\codes\ProgrammerLearning\testrepo> git branch testbranch
PS D:\codes\ProgrammerLearning\testrepo> git branch  
\* main  
  testbranch

Now there're two branches - main and testbranch

### switch branch

use "git switch" to change between branches
```powershll
PS D:\codes\ProgrammerLearning\testrepo> git switch testbranch
M       first.md  
Switched to branch 'testbranch'  
PS D:\codes\ProgrammerLearning\testrepo> git branch  
  main  
\* testbranch

### commit to branch
```powershell
PS D:\codes\ProgrammerLearning\testrepo> git branch  
  main  
\* testbranch
PS D:\codes\ProgrammerLearning\testrepo> git add .
PS D:\codes\ProgrammerLearning\testrepo> git status
On branch testbranch
Changes to be committed:
  (use "git restore --staged <file>..." to unstage)
        modified:   first.md

PS D:\codes\ProgrammerLearning\testrepo> git commit -m "first commit to branch after learning"
[testbranch 50c7bfc] first commit to branch after learning
 Committer: Yang <yangh@avaya.com>
Your name and email address were configured automatically based
on your username and hostname. Please check that they are accurate.
You can suppress this message by setting them explicitly. Run the
following command and follow the instructions in your editor to edit
your configuration file:

    git config --global --edit

After doing this, you may fix the identity used for this commit with:

    git commit --amend --reset-author

 1 file changed, 0 insertions(+), 0 deletions(-)

 
