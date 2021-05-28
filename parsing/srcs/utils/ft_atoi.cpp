#include "../../includes/utils.hpp"

int		ft_atoi(const char *s)
{
	int		ret;
	int		sign;

	ret = 0;
	sign = 1;
	while ((*s >= 9 && *s <= 13) || (*s == ' '))
		s++;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}
	while (*s >= '0' && *s <= '9')
	{
		ret = ret * 10 + (*s - 48);
		s++;
	}
	return (ret * sign);
}
