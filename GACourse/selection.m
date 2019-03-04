function [parent1, parent2] = selection(population)

M = length(population.Chromosomes(:));

normalized_fitness = [population.Chromosomes(:).fitness] ./ sum([population.Chromosomes(:).fitness]);

[sorted_fintness_values , sorted_idx] = sort(normalized_fitness , 'descend');

for i = 1 : length(population.Chromosomes)
    temp_population.Chromosomes(i).Gene = population.Chromosomes(sorted_idx(i)).Gene;
    temp_population.Chromosomes(i).fitness = population.Chromosomes(sorted_idx(i)).fitness;
    temp_population.Chromosomes(i).normalized_fitness = normalized_fitness(sorted_idx(i));
end


cumsum = zeros(1 , M);

for i = 1 : M
    for j = i : M
        cumsum(i) = cumsum(i) + temp_population.Chromosomes(j).normalized_fitness;
    end
end


R = rand(); % in [0,1]
parent1_idx = M;
for i = 1: length(cumsum)
    if R > cumsum(i)
        parent1_idx = i - 1;
        break;
    end
end

parent2_idx = parent1_idx;

while parent2_idx == parent1_idx
    R = rand(); % in [0,1]

    for i = 1: length(cumsum)
        if R > cumsum(i)
            parent2_idx = i - 1;
            break;
        end
    end
end

parent1 = temp_population.Chromosomes(parent1_idx);
parent2 = temp_population.Chromosomes(parent2_idx);

end