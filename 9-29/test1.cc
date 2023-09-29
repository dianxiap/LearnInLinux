#include <iostream>
#include <string>

using namespace std;

string s;

int main()
{
  cin>>s;
  string res;
  for(int i=0;i<s.size();i++)
  {
    int j=i;
    string tmp;
    while(s[j]>='0'&&s[j]<='9'&&j<s.size())
    {
      tmp.push_back(s[j]);
      j++;
    }
    if(tmp.size()>res.size())
    {
      res=tmp;
    }
    tmp="";
  }
  cout<<res<<endl;
  return 0;
}

