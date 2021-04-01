/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ps_tester.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/30 08:26:04 by dait-atm          #+#    #+#             */
/*   Updated: 2021/03/31 10:30:29 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ps_tester.h"

# define N 100

void	new_rand_req(int *ar, int j, int *r)
{
	int	i;

	*r = rand();	// MAXIMUM here
	i = 0;
	while (i < j)
	{
		if (ar[i] == *r)
			new_rand_req(ar, j, r);
		i++;
	}
	return ;
}

int	new_rand(int *ar, int j)
{
	int	r;

	new_rand_req(ar, j, &r);
	return (r);
}

int	*init_array(int size)
{
	int	*ar;
	int	i;
	int	j;

	if (!(ar = malloc(sizeof(int) * size)))
		exit(0);
	j = 0;
	while (j < size)
	{
		ar[j] = new_rand(ar, j);
		j++;
	}
	return (ar);
}

void	str_clear(char *as[N])
{
	int	i;

	i = 0;
	while ((as)[i])
		free((as)[i++]);
}

int asprintf(char **strp, const char *fmt, ...);

char	*arg_str(int *ar, int size)
{
	int		i;
	char	*s;
	char	*tmp;

	s = strdup("../push_swap ");
	i = 0;
	while (i < size)
	{
		tmp = s;
		asprintf(&s, "%s%d ", s, ar[i]);
		free(tmp);
		i++;
	}
	return (s);
}

int	exec_ps(char *as, char **env, int *cpt)
{
	char	l[16];
	FILE 	*r;
	FILE	*f;

	f = fopen("tmp.log", "w");
	r = popen(as, "r");
	while (fgets(l, 16, r))
	{
		(*cpt)++;
		fprintf(f, "%s",l);
	}
	pclose(r);
	fclose(f);
	return (0);
}

int	exec_ch(char *as, char **env, int cpt)
{
	char	l[16];
	FILE 	*r;
	FILE	*f;

	int		sout[2];
	int		ret;

	strncpy(as + 3, "checker  ", 9);

	if (!(f = fopen("tmp.log", "r")))
	{
		dprintf(2, "can't open tmp file\n");
		return (1);
	}
	pipe(sout);
	ret = dup(1);
	dup2(sout[1], 1);
	r = popen(as, "w");
	while (fgets(l, 16, f))
		fprintf(r, "%s", l);
	pclose(r);
	fclose(f);
	read(sout[0], l, 16);
	dup2(ret, 1);
	close(ret);
	close(sout[0]);
	close(sout[1]);
	if (strncmp(l, "OK\n", 3))
	{
		f = fopen("tmp.log", "a");
		fprintf(f, "\n\nreturn : \n%s\ncommand used : \n%s\n", l, as);
		fclose(f);
		dprintf(2, "KO or Error : see 'tmp.log' to find more inforamtions\n");
		return (1);
	}
	remove("tmp.log");
	return (0);
}

void	min_max_avg(unsigned long long t[3], int *cpt)
{
	int	i;

	t[0] = 0;
	t[1] = 0;
	t[2] = cpt[0];
	i = 1;
	while (i < N)
	{
		if (cpt[i] < cpt[t[0]])
			t[0] = i;
		else if (cpt[i] > cpt[t[1]])
			t[1] = i;
		t[2] += cpt[i];
		i++;
	}
	if (errno == ERANGE)
		t[2] = 0;
	else
		t[2] = (unsigned long long)(t[2] / (int)N);
}

void	show_stat(int *cpt)
{
	FILE				*f;
	int					i;
	unsigned long long	t[3];

	min_max_avg(t, cpt);
	f = fopen("operations_data.js", "w");

	fprintf(f, "var operations_minimum = %d;\n", cpt[t[0]]);
	fprintf(f, "var operations_maximum = %d;\n", cpt[t[1]]);
	fprintf(f, "var operations_avg = %lld;\n", t[2]);

	fprintf(f, "var array = [\n");
	i = -1;
	while (++i < N - 1)
	{
		if (i == t[0])
			fprintf(f, "{ x: %d, y: %d, indexLabel: \"\u2193\", markerColor: \"cyan\", markerType: \"cross\"},\n", i, cpt[i]);
		else if (i == t[1])
			fprintf(f, "{ x: %d, y: %d, indexLabel: \"\u2191\", markerColor: \"yellow\", markerType: \"cross\"},\n", i, cpt[i]);
		else
			fprintf(f, "{ x: %d, y: %d},\n", i, cpt[i]);
	}
	fprintf(f, "{ x: %d, y: %d}\n", i, cpt[i]);
	fprintf(f, "]\n");
	fclose(f);
	system("xdg-open ./index.html");
}

void	main(int argc, char **argv, char **env)
{
	int		size;
	int		*ar[N];
	char	*as[N + 1];
	int		cpt[N];
	int		i;

	if (argc < 2)
		exit(1);
	srand(time (NULL));
	errno = 0;
	size = (int)strtoll(argv[1], NULL, 10);
	if (errno || size <= 0)
		exit(1);

	printf("processing...\n");
	memset(cpt, 0, sizeof(int) * N);
	memset(as, 0, sizeof(char *) * (N + 1));
	i = 0;
	while (i < N)
	{
		ar[i] = init_array(size);

		as[i] = arg_str(ar[i], size);
		free(ar[i]);

		exec_ps(as[i], env, &cpt[i]);

		exec_ch(as[i], env, cpt[i]);

		i++;
	}
	show_stat(cpt);
	printf("done\n");
	str_clear(as);
}
