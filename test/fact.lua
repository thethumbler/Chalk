function fact(n)
	if n ~= 0 then 
		return n * fact(n-1)
	end
	return 1
end

print(fact(10))