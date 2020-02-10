/*
 * 
 * nums = [-1, 0, 1, 2, -1, -4],

solution:
[
  [-1, 0, 1],
  [-1, -1, 2]
]*/

vector< vector<int>> threeSum(vector<int>& nums) {
        vector< vector<int> > triplet;
        
        sort(nums.begin(),nums.end());
        
        unsigned int first = nums[0], second = nums[1], third = nums.size()-1;       
        
        if(nums[first] > 0 && nums[third] > 0){
          return {};
        }
        else if(nums[first] < 0U && nums[third] < 0U){
          return {};
        }
        else{
          for(int i = 0;i < nums.size()-2; i++){
            
          }
            while(second < third){
              int value = first + second + third;
              
              if(!value){
                triplet[i].push_back(first);
                triplet[i].push_back(second);
                triplet[i].push_back(third);
              }
              else if(value > 0){
                --third;
              }
              else{
                ++second;
              }
            }
        }
    }