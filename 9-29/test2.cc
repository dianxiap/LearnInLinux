class Solution 
{
public:
  int a[50010];
  int MoreThanHalfNum_Solution(vector<int>& numbers)
  {
    int n=numbers.size();
    for(int i=0;i<n;i++)
    {
      a[numbers[i]]++;
      if(a[numbers[i]]>n/2)
        return numbers[i];
    }
    return false;
  }
};
