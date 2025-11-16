struct ArrayGenerator {
   mt19937 rng;
   uniform_int_distribution<int> dist;
   int maxN;
   vector<int> baseRandom;
   vector<int> baseReverse;
   vector<int> baseNearly;
   ArrayGenerator(int maxN_, int valueMax)
      : rng(1234567), dist(0, valueMax), maxN(maxN_) {
      baseRandom.resize(maxN);
      for (int i = 0; i < maxN; ++i) baseRandom[i] = dist(rng);
      baseReverse = baseRandom;
      sort(baseReverse.begin(), baseReverse.end(), greater<int>());
      vector<int> sortedAsc = baseRandom;
      sort(sortedAsc.begin(), sortedAsc.end());
      baseNearly = sortedAsc;
      int swaps = max(1, maxN / 100); // ~1% случайных обменов
      uniform_int_distribution<int> pos(0, maxN - 1);
      for (int s = 0; s < swaps; ++s) {
         int i = pos(rng);
         int j = pos(rng);
         if (i != j) swap(baseNearly[i], baseNearly[j]);
      }
   }
   vector<int> getRandomPrefix(int n) const {
      return vector<int>(baseRandom.begin(), baseRandom.begin() + n);
   }
   vector<int> getReversePrefix(int n) const {
      return vector<int>(baseReverse.begin(), baseReverse.begin() + n);
   }
   vector<int> getNearlyPrefix(int n) const {
      return vector<int>(baseNearly.begin(), baseNearly.begin() + n);
   }
};

struct SortTester {
   int repeats;
   SortTester(int repeats_) : repeats(repeats_) {}
   long long time_us(function<void(vector<int>&)> sorter, vector<int> a) {
      using namespace std::chrono;
      auto start = high_resolution_clock::now();
      sorter(a);
      auto elapsed = duration_cast<microseconds>(high_resolution_clock::now() - start).count();
      return (long long)elapsed;
   }
   void run(ArrayGenerator& gen, const vector<int>& sizes, const vector<int>& thresholds) {
      cout << "dataset,size,algo,threshold,trial,time_us\n";
      for (int n : sizes) {
         // Random
         for (int t = 0; t < repeats; ++t) {
            auto arr = gen.getRandomPrefix(n);
            long long us = time_us([](vector<int>& v){ Merge(v); }, arr);
            cout << "random," << n << ",ms,0," << t+1 << "," << us << "\n";
         }
         for (int thr : thresholds) {
            for (int t = 0; t < repeats; ++t) {
               auto arr = gen.getRandomPrefix(n);
               long long us = time_us([&](vector<int>& v){ MergeWithThreshold(v, thr); }, arr);
               cout << "random," << n << ",ms+ins," << thr << "," << t+1 << "," << us << "\n";
            }
         }
         // Reverse
         for (int t = 0; t < repeats; ++t) {
            auto arr = gen.getReversePrefix(n);
            long long us = time_us([](vector<int>& v){ Merge(v); }, arr);
            cout << "reverse," << n << ",ms,0," << t+1 << "," << us << "\n";
         }
         for (int thr : thresholds) {
            for (int t = 0; t < repeats; ++t) {
               auto arr = gen.getReversePrefix(n);
               long long us = time_us([&](vector<int>& v){ MergeWithThreshold(v, thr); }, arr);
               cout << "reverse," << n << ",ms+ins," << thr << "," << t+1 << "," << us << "\n";
            }
         }
         // Nearly-sorted
         for (int t = 0; t < repeats; ++t) {
            auto arr = gen.getNearlyPrefix(n);
            long long us = time_us([](vector<int>& v){ Merge(v); }, arr);
            cout << "nearly," << n << ",ms,0," << t+1 << "," << us << "\n";
         }
         for (int thr : thresholds) {
            for (int t = 0; t < repeats; ++t) {
               auto arr = gen.getNearlyPrefix(n);
               long long us = time_us([&](vector<int>& v){ MergeWithThreshold(v, thr); }, arr);
               cout << "nearly," << n << ",ms+ins," << thr << "," << t+1 << "," << us << "\n";
            }
         }
      }
   }
};
