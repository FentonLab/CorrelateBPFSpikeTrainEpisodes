	for(y=0; y < delta_y; y++){
		for(x=0; x < delta_x; x++){
			distance = hypot(y, x) + 0.5;
			if(distance > delta_y){
				weight[y][x] = 0.0;
				continue;
			}
			weight[y][x] = distance;
			sum += weight[y][x];
		}
	}
	for(y=0; y < delta_y; y++)
		for(x=0; x < delta_x; x++)
			weight[y][x] /= sum[y][x];

	for(Y=0; Y < COARSE_Y; Y++){
		for(X=0; X < COARSE_X; X++){
			max_y = Y+delta_y;
			if(max_y > FINE_Y)
				max_y = FINE_Y;
			for(y=Y-delta_y; y < max_y; y++){
				if(y < 0)
					continue;
				max_x = X+delta_x;
				if(max_x > FINE_X)
					max_x = FINE_X;
				for(x=X-delta_x; x < max_x; x++){
					if(x < 0)
						continue;
					spikes[Y][X] += (spikes[y][x] * weight[dy][dx]);
					time[Y][X] += (time[y][x]* weight[dy][dx] );
				}
			}
		}
	}
	for(Y=0; Y < COARSE_Y; Y++){
		for(X=0; X < COARSE_X; X++){
			rate[Y][X] = spikes[Y][X] / (time[Y][X] * samps_per_sec);
		}
	}
			
