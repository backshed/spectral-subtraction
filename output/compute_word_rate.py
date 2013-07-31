#!/bin/python3.3
# -*- coding: utf-8 -*-
# Script to compute word recognition rate.



def ComputeWRR(words_origin, words_result):
	'''Parameters are the twenty sentences cut in words only'''
	max_inc = 3
	WRR = 0
	for i in range(0, 20):
		N = len(words_origin[i])
		S = 0
		D = 0
		I = 0
		C = 0

		#~ print(words_origin[i])
		#~ print(words_result[i])

		origin_cursor = 0
		result_cursor = 0
		# on va essayer d'aller le plus loin possible jusqu'a une erreur
		for j in range(max(N, len(words_result[i]))):
			if words_result[i][result_cursor] == words_origin[i][origin_cursor]:
				C += 1
			else:
				leave = False
				# Erreur. Trois cas :
				# Substitution : on a le meme nombre de mots mais les mots sont !=
				for k in range(1, max_inc):
					if (result_cursor + k < len(words_result[i]) and
						origin_cursor + k < len(words_origin[i]) and
						words_result[i][result_cursor + k] == words_origin[i][origin_cursor + k]):

						result_cursor += k
						origin_cursor += k
						S += k
						leave = True

				if(leave):
					leave = False
					result_cursor += 1
					origin_cursor += 1
					if (result_cursor >= len(words_result[i]) or
						origin_cursor >= len(words_origin[i])):
						break
					continue


				# Suppression : result_cursor arrive sur un mot présent a origin_cursor + 1
				for k in range(1, max_inc):
					if (origin_cursor + k < len(words_origin[i]) and
					    words_result[i][result_cursor] == words_origin[i][origin_cursor + k]):

						origin_cursor += k
						D += k
						leave = True

				if(leave):
					leave = False
					result_cursor += 1
					origin_cursor += 1
					if (result_cursor >= len(words_result[i]) or
						origin_cursor >= len(words_origin[i])):
						break
					continue

				# Insertion : origin_cursor arrive sur un mot présent a result_cursor + 1
				for k in range(1, max_inc):
					if (result_cursor + k < len(words_result[i]) and
						words_result[i][result_cursor + k] == words_origin[i][origin_cursor]):

						result_cursor += k
						I += k
						leave = True

				if(leave):
					leave = False
					result_cursor += 1
					origin_cursor += 1
					if (result_cursor >= len(words_result[i]) or
						origin_cursor >= len(words_origin[i])):
						break
					continue

				# Cas plus rare : si deux glyphes ont été fusionnées.
				# Il faut vérifier si result_cursor == origin_cursor . origin_curosor + 1
				if (origin_cursor + 1 < len(words_origin[i]) and
					words_result[i][result_cursor] == (words_origin[i][origin_cursor] + words_origin[i][origin_cursor + 1])):
					#~ print("concat error")
					origin_cursor += 1

				# On ne devrait pas arriver la : incrementer max_inc ?


			result_cursor += 1
			origin_cursor += 1
			if (result_cursor >= len(words_result[i]) or
				origin_cursor >= len(words_origin[i])):
				break
		WRR += (S + D + I) / N
		#~ print("Sentence " + str(i) + " WRR : " + str(WRR))

	return WRR / 20 # median wrr for all the files
