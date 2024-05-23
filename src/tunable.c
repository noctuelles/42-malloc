/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tunable.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 17:06:00 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/23 17:09:41 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

size_t
get_tunable(const char *name, size_t default_value) {
    char *env_var = NULL;

    env_var = getenv(name);
    if (env_var == NULL) {
        return (default_value);
    }
    return (atoll(env_var));
}