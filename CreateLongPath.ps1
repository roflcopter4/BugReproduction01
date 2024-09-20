$long_name = "$PSScriptRoot\" +
    'this_is_a_long_sequence_of_stream_of_consciousness_words_that_I_mostly_thought_up_off_the_top_of_my_head\' + 
    'in_rapid_succession_and_I_cant_really_use_punctuation_or_at_least_not_quotation_marks\' + 
    'actually_scratch_that_I_could_use_all_the_rest_of_the_punctuation,\' +
    'not_sure_why_I_had_thought_otherwise_for_that_moment\' +
    'though_I_suppose_its_more_interesting_without_the_punctuation_if_that_makes_any_sense\' +
    '🔥😁🚨'

mkdir "$long_name" >NUL 2>&1

Push-Location
Set-Location "$long_name"
Write-Output 'Hello, world!' > foo.txt
Write-Output 'Goodbye, world!' > bar.txt
Pop-Location
